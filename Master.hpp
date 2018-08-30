#ifndef JM_MASTER_HPP
#define JM_MASTER_HPP

#include "ArgManager.hpp"
#include "DataProcessor.hpp"
#include "DataQueue.hpp"
#include "DataReader.hpp"
#include "DataWriter.hpp"


class Master {
private:
    DataQueue m_queue_in;
    DataQueue m_queue_out;
    DataReader    *m_reader;
    DataProcessor *m_processor;
    DataWriter    *m_writer;
    std::atomic<Status> m_status {Status::reading};

public:
    Master(ArgManager& arg_manager);
    void start_workers();
    bool workers_done();
    Status getStatus() const;
    ~Master();
};

Master::Master(ArgManager& arg_manager)
{
    m_reader    = new DataReader(m_status, m_queue_in);
    m_processor = new DataProcessor(m_status, m_queue_in, m_queue_out, arg_manager);
    m_writer    = new DataWriter(m_status, m_queue_out, arg_manager);
}

Master::~Master()
{
    delete m_writer;
    delete m_processor;
    delete m_reader;
}

void Master::start_workers()
{
    m_reader->start();
    m_processor->start();
    m_writer->start();
}

bool Master::workers_done()
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
