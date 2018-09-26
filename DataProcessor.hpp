#ifndef JM_DATA_PROCESSOR_HPP
#define JM_DATA_PROCESSOR_HPP

#include "DataQueue.hpp"
#include "Line.hpp"
#include "Worker.hpp"

class DataProcessor : public Worker {
public:
    DataProcessor(const Arguments& args, DataQueue& queue_in, DataQueue& queue_out);

private:
    DataQueue& m_queue_in;
    DataQueue& m_queue_out;
    std::atomic<unsigned> m_count_started{0};
    std::atomic<unsigned> m_count_ended{0};

private:
    void doJob();
    void processLine();
};

DataProcessor::DataProcessor(const Arguments& args, DataQueue& queue_in, DataQueue& queue_out) :
    Worker(args), m_queue_in(queue_in), m_queue_out(queue_out)
{
}

void DataProcessor::doJob()
{
    while (!m_done) {
        processLine();

        if (m_status == Status::processing && m_count_started == m_count_ended && m_queue_in.size() == 0) {
            // Note that we can finish even if m_count_started == 0.
            // This means that another worker(s) did all the job.
            m_done = true;
        }
    }

    return;
}

void DataProcessor::processLine()
{
    auto line = m_queue_in.pullNext();

    if (!line.isEmpty()) {
        ++m_count_started;
        line.process(m_args);
        m_queue_out.push(line);
        ++m_count_ended;
    }

    return;
}

#endif //JM_DATA_PROCESSOR_HPP
