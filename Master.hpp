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
    std::atomic<Status> m_status {Status::reading};
    unsigned m_num_processors;

public:
    Master(const Arguments& args);
    void startWorkers();
    bool workersDone();
    Status getStatus() const;
};

Master::Master(const Arguments& args)
{
    auto m_num_processors = std::max(std::thread::hardware_concurrency() -2, 1u);

    // Spawn Reader
    m_workers.push_back(std::make_shared<DataReader>(m_status, args, m_queue_in));

    // Spawn Writer
    m_workers.push_back(std::make_shared<DataWriter>(m_status, args, m_queue_out));

    // Spawn Processors
    for (auto i = 0u; i<m_num_processors; ++i) {
        m_workers.push_back(std::make_shared<DataProcessor>(m_status, args, m_queue_in, m_queue_out));
    }
}

void Master::startWorkers()
{
    for (const auto& worker : m_workers) {
        worker->start();
    }
}

bool Master::workersDone()
{
    auto done = false;

    // Workers are not "done" until the workers that provide them with data are done.
    auto done_count = std::count_if(m_workers.begin(), m_workers.end(), [](std::shared_ptr<Worker>& w){return w->done();});

    if (done_count == 0) {
        m_status = Status::reading;
    } else if (done_count == 1) {
        m_status = Status::processing;
    } else if (done_count == m_num_processors + 1) {
        m_status = Status::writing;
    } else {
        done = true;
    }

    return done;
}

#endif //JM_MASTER_HPP
