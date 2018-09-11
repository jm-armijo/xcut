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
    std::shared_ptr<Worker> m_reader;
    std::shared_ptr<Worker> m_processor;
    std::shared_ptr<Worker> m_writer;
    std::atomic<Status> m_status {Status::reading};

public:
    Master(const Arguments& args);
    void startWorkers();
    bool workersDone();
    Status getStatus() const;
};

Master::Master(const Arguments& args)
{
    m_reader    = std::make_shared<DataReader>   (m_status, args, m_queue_in);
    m_processor = std::make_shared<DataProcessor>(m_status, args, m_queue_in, m_queue_out);
    m_writer    = std::make_shared<DataWriter>   (m_status, args, m_queue_out);
}

void Master::startWorkers()
{
    m_reader->start();
    m_processor->start();
    m_writer->start();
}

bool Master::workersDone()
{
    auto done = false;
    switch (m_status) {
        case Status::reading:
            if (m_reader->done()) {
                m_status = Status::processing;
            }
            break;
        case Status::processing:
            if (m_processor->done()) {
                m_status = Status::writing;
            }
            break;
        case Status::writing:
            if (m_writer->done()) {
                done = true;
            }
    }

    return done;
}

#endif //JM_MASTER_HPP
