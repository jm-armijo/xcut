#ifndef JM_INPUT_READER_HPP
#define JM_INPUT_READER_HPP

#include <thread>

#include "DataQueue.hpp"
#include "Line.hpp"

class DataProcessor {
public:
    DataProcessor(DataQueue& queue_in, DataQueue& queue_out, ArgManager& arg_manager);
    void do_job();
    ~DataProcessor();

private:
    std::vector<std::thread> m_threads;
    std::thread m_thread;
    DataQueue& m_queue_in;
    DataQueue& m_queue_out;
    ArgManager& m_arg_manager;

private:
    void spawn();
    void processLine(unsigned line_num, std::string src_line);
};

DataProcessor::DataProcessor(DataQueue& queue_in, DataQueue& queue_out, ArgManager& arg_manager) :
    m_queue_in(queue_in), m_queue_out(queue_out), m_arg_manager(arg_manager)
{
}

DataProcessor::~DataProcessor()
{
    // Wait until all input is processed
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void DataProcessor::do_job()
{    
    m_thread = std::thread(&DataProcessor::spawn, this);
}

void DataProcessor::spawn()
{
    auto line_num = 0u;

    // If stopped reading we may still have some items to process
    while (!m_queue_in.is_eof() || m_queue_in.size() > 0) {
        if (m_queue_in.size() > 0) {
            auto val = m_queue_in.pull();
            m_threads.push_back(std::thread(&DataProcessor::processLine, this, line_num++, val));
        }
    }

    // Wait for all threads to finish.
    for (auto& t : m_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    m_queue_out.set_eof();

    return;
}

void DataProcessor::processLine(unsigned line_num, std::string src_line)
{
    Line line(src_line);
    std::string new_line = line.process(m_arg_manager);
    m_queue_out.push(new_line, line_num);

    return;
}

#endif //JM_INPUT_READER_HPP
