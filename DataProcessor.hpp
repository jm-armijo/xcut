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

private:
    DataProcessor() = delete;
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

        if (m_status == Status::processing && m_queue_in.getCountIn() == m_queue_out.getCountIn()) {
            m_done = true;
        }
    }

    return;
}

void DataProcessor::processLine()
{
    auto line = m_queue_in.pullNext();

    if (!line.isEmpty()) {
        line.process(m_args);
        m_queue_out.push(line);
    }

    return;
}

#endif //JM_DATA_PROCESSOR_HPP
