#ifndef JM_DATA_WRITER_HPP
#define JM_DATA_WRITER_HPP

#include <iostream>
#include <thread>

#include "ArgManager.hpp"
#include "DataQueue.hpp"

class DataWriter {
public:
    DataWriter(DataQueue& queue, ArgManager& arg_manager);
    void do_job();
    void push(const std::string& value, unsigned line_num);
    ~DataWriter();

private:
    DataQueue& m_queue;
    std::thread m_thread;
    ArgManager& m_arg_manager;

private:
    DataWriter() = delete;
    void writeStream();
    void printOutputSorted();
    void printOutputUnsorted();
};

DataWriter::DataWriter(DataQueue& queue, ArgManager& arg_manager) :
    m_queue(queue), m_arg_manager(arg_manager)
{
}

void DataWriter::do_job()
{
    m_thread = std::thread(&DataWriter::writeStream, this);
}

DataWriter::~DataWriter()
{
    // Wait until all input is read
    while(true) {
        if (m_queue.is_eof() && m_queue.size() == 0) {
            break;
        }
    }

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void DataWriter::writeStream()
{
    while (!m_queue.is_eof() || m_queue.size() > 0) {
        if (m_arg_manager.sortOutput()) {
            printOutputSorted();
        } else {
            printOutputUnsorted();
        }
    }

    return;
}

void DataWriter::printOutputSorted()
{
    static auto line_num = 0u;
    if (m_queue.exists(line_num)) {
        auto val = m_queue.pull(line_num);
        ++line_num;
        std::cout << val << "\n";
    }

    return;
}

void DataWriter::printOutputUnsorted()
{
    if (m_queue.size() > 0) {
        auto val = m_queue.pull();
        std::cout << val << "\n";
    }

    return;
}

#endif //JM_DATA_WRITER_HPP
