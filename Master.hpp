#ifndef JM_MASTER_HPP
#define JM_MASTER_HPP

#include <algorithm>
#include <memory>

#include "ArgManager.hpp"
#include "DataProcessor.hpp"
#include "DataQueue.hpp"
#include "DataReader.hpp"
#include "DataWriter.hpp"


class Master {
private:
    DataQueue m_queue_in;
    DataQueue m_queue_out;
    std::vector<std::shared_ptr<Worker>> m_workers;
    Status m_status = Status::reading;
    const unsigned m_num_reading_workers;
    const unsigned m_num_process_workers;
    const unsigned m_num_writing_workers;

public:
    Master(const Arguments& args);
    void startWorkers();
    bool workersDone();

private:
    void checkStatus();
    void notifyWorkers();

};

Master::Master(const Arguments& args) :
    m_num_reading_workers(1),
    m_num_process_workers(std::max(std::thread::hardware_concurrency() -2, 1u)),
    m_num_writing_workers(1)
{

    // Spawn Reader
    m_workers.push_back(std::make_shared<DataReader>(args, m_queue_in));

    // Spawn Processors
    for (auto i = 0u; i<m_num_process_workers; ++i) {
        m_workers.push_back(std::make_shared<DataProcessor>(args, m_queue_in, m_queue_out));
    }

    // Spawn Writer
    m_workers.push_back(std::make_shared<DataWriter>(args, m_queue_out));
}

void Master::startWorkers()
{
    for (const auto& worker : m_workers) {
        worker->start();
    }
}

void Master::notifyWorkers()
{
    for (const auto& worker : m_workers) {
        worker->update(m_status);
    }
}

void Master::checkStatus()
{
    static const auto expected_reading    = m_num_reading_workers;
    static const auto expected_processing = expected_reading    + m_num_process_workers;
    static const auto expected_writing    = expected_processing + m_num_writing_workers;

    // Workers are not "done" until the workers that feed them with data are done.
    auto done_count = std::count_if(m_workers.begin(), m_workers.end(), [](std::shared_ptr<Worker>& w){return w->done();});

    Status prev_status = m_status;
    if (m_status == Status::reading && done_count == expected_reading) {
        m_status = Status::processing;
    } else if (m_status == Status::processing && done_count == expected_processing) {
        m_status = Status::writing;
    } else if (m_status == Status::writing && done_count == expected_writing) {
        m_status = Status::done;
    }

    // Notify workers only if the status changed
    if (prev_status != m_status) {
        notifyWorkers();
    }
}

bool Master::workersDone()
{
    checkStatus();
    return m_status == Status::done;
}

#endif //JM_MASTER_HPP
