#ifndef JM_DATA_PROCESSOR_HPP
#define JM_DATA_PROCESSOR_HPP

#include "DataQueue.hpp"
#include "Line.hpp"
#include "Worker.hpp"

class DataProcessor : public Worker {
public:
    DataProcessor(const std::atomic<Status> &status, DataQueue& queue_in, DataQueue& queue_out, const Arguments& args);
    ~DataProcessor();

private:
    DataQueue& m_queue_in;
    DataQueue& m_queue_out;
    std::vector<std::thread> m_threads;
    std::atomic<unsigned> m_count_started{0};
    std::atomic<unsigned> m_count_ended{0};
    const Arguments& m_args;
    std::mutex m_mtx_queue;

private:
    void do_job();
    void processLines();
    void processLine();
};

DataProcessor::DataProcessor(const std::atomic<Status> &status, DataQueue& queue_in, DataQueue& queue_out, const Arguments& args) :
    Worker(status), m_queue_in(queue_in), m_queue_out(queue_out), m_args(args)
{
}


void DataProcessor::processLines()
{
    while (!m_done) {
        processLine();
    }
}

void DataProcessor::do_job()
{
    auto num_threads = std::thread::hardware_concurrency();
    for(auto i = 0u; i<num_threads; ++i) {
        m_threads.push_back(std::thread(&DataProcessor::processLines, this));
    }

    while (m_status == Status::reading);
    while (m_queue_in.size() > 0 || m_count_started > m_count_ended);

    m_done = true;

    return;
}

void DataProcessor::processLine()
{
    auto process = false;
    auto line_num = 0u;
    auto src_line = std::string();

    m_mtx_queue.lock();
    if (m_queue_in.size() > 0) {
        ++m_count_started;
        line_num = m_queue_in.nextKey();
        src_line = m_queue_in.pull(line_num);
        process = true;
    }
    m_mtx_queue.unlock();

    if (process) {
        Line line(src_line);
        std::string new_line = line.process(m_args);
        m_queue_out.push(new_line, line_num);
        ++m_count_ended;
    }

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
