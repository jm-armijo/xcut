#ifndef JM_MASTER_HPP
#define JM_MASTER_HPP

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

public:
    Master(const Arguments& args);
    void startWorkers();
    bool workersDone();
    Status getStatus() const;
};

Master::Master(const Arguments& args)
{
    m_workers.push_back(std::make_shared<DataReader>   (m_status, args, m_queue_in));
    m_workers.push_back(std::make_shared<DataProcessor>(m_status, args, m_queue_in, m_queue_out));
    m_workers.push_back(std::make_shared<DataWriter>   (m_status, args, m_queue_out));
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
    auto done_count = std::count_if(m_workers.begin(), m_workers.end(), [](std::shared_ptr<Worker>& w){return w->done();});

    switch (done_count) {
        case 3:
            done = true;
            break;
        case 2:
            m_status = Status::writing;
            break;
        case 1:
            m_status = Status::processing;
            break;
        default:
            m_status = Status::reading;
            break;
    }

    return done;
}

#endif //JM_MASTER_HPP
