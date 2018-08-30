#ifndef JM_DATA_WRITER_HPP
#define JM_DATA_WRITER_HPP

#include "ArgManager.hpp"
#include "DataQueue.hpp"
#include "Worker.hpp"

class DataWriter : public Worker {
public:
    DataWriter(const std::atomic<Status> &status, DataQueue& queue, ArgManager& arg_manager);
    void push(const std::string& value, unsigned line_num);

private:
    DataQueue& m_queue;
    ArgManager& m_arg_manager;

private:
    DataWriter() = delete;
    void do_job();
    void printOutputSorted();
    void printOutputUnsorted();
};

DataWriter::DataWriter(const std::atomic<Status> &status, DataQueue& queue, ArgManager& arg_manager) :
    Worker(status), m_queue(queue), m_arg_manager(arg_manager)
{
}

void DataWriter::do_job()
{
    while (m_status != Status::writing || m_queue.size() > 0) {
        if (m_arg_manager.sortOutput()) {
            printOutputSorted();
        } else {
            printOutputUnsorted();
        }
    }

    m_done = true;

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
        auto line_num = m_queue.nextKey();
        auto value    = m_queue.pull(line_num);
        std::cout << value << "\n";
    }

    return;
}

#endif //JM_DATA_WRITER_HPP
