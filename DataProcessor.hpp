#ifndef JM_DATA_PROCESSOR_HPP
#define JM_DATA_PROCESSOR_HPP

#include "DataQueue.hpp"
#include "Line.hpp"
#include "Worker.hpp"

class DataProcessor : public Worker {
public:
    DataProcessor(DataQueue& queue_in, DataQueue& queue_out, ArgManager& arg_manager);
    ~DataProcessor();

private:
    DataQueue& m_queue_in;
    DataQueue& m_queue_out;
    std::vector<std::thread> m_threads;
    std::atomic<unsigned> m_count_started{0};
    std::atomic<unsigned> m_count_ended{0};
    ArgManager& m_arg_manager;

private:
    void do_job();
    void processLine(unsigned line_num, std::string src_line);
};

DataProcessor::DataProcessor(DataQueue& queue_in, DataQueue& queue_out, ArgManager& arg_manager) :
    m_queue_in(queue_in), m_queue_out(queue_out), m_arg_manager(arg_manager)
{
}

void DataProcessor::do_job()
{

    // If stopped reading we may still have some items to process
    while (!m_queue_in.is_eof() || m_queue_in.size() > 0) {
        if (m_queue_in.size() > 0) {
            auto line_num = m_queue_in.nextKey();
            auto value    = m_queue_in.pull(line_num);
            ++m_count_started;
            m_threads.push_back(std::thread(&DataProcessor::processLine, this, line_num, value));
        }
    }

    while (m_count_started > m_count_ended);

    m_done = true;
    m_queue_out.set_eof();

    return;
}

void DataProcessor::processLine(unsigned line_num, std::string src_line)
{
    Line line(src_line);
    std::string new_line = line.process(m_arg_manager);
    m_queue_out.push(new_line, line_num);
    ++m_count_ended;

    return;
}

DataProcessor::~DataProcessor()
{
    // Wait for all threads to finish.
    for (auto& t : m_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

#endif //JM_DATA_PROCESSOR_HPP
