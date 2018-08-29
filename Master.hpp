#include "ArgManager.hpp"
#include "DataProcessor.hpp"
#include "DataQueue.hpp"
#include "DataReader.hpp"
#include "DataWriter.hpp"


class Master {
private:
    enum class Status {reading, processing, writing};
    DataQueue m_queue_in;
    DataQueue m_queue_out;
    DataReader    *m_reader;
    DataProcessor *m_processor;
    DataWriter    *m_writer;
    Status m_status = Status::reading;

public:
    Master(ArgManager& arg_manager);
    void start_workers();
    bool workers_done();
    ~Master();
};

Master::Master(ArgManager& arg_manager)
{
    m_reader    = new DataReader(m_queue_in);
    m_processor = new DataProcessor(m_queue_in, m_queue_out, arg_manager);
    m_writer    = new DataWriter(m_queue_out, arg_manager);
}

Master::~Master()
{
    delete m_writer;
    delete m_processor;
    delete m_reader;
}

void Master::start_workers()
{
    m_reader->do_job();
    m_processor->do_job();
    m_writer->do_job();
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

