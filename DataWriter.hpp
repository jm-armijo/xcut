#ifndef JM_DATA_WRITER_HPP
#define JM_DATA_WRITER_HPP

#include "ArgManager.hpp"
#include "DataQueue.hpp"
#include "Worker.hpp"

class DataWriter : public Worker {
public:
    DataWriter(const Arguments& args, DataQueue& queue);

private:
    DataQueue& m_queue;

private:
    DataWriter() = delete;
    void doJob();
    void printOutputSorted();
    void printOutputUnsorted();
};

DataWriter::DataWriter(const Arguments& args, DataQueue& queue) :
    Worker(args), m_queue(queue)
{
}

void DataWriter::doJob()
{
    while (m_status != Status::writing || m_queue.size() > 0) {
        if (m_args.get("-s") == "1") {
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
    static auto line_num = 1u;
    auto line = m_queue.pull(line_num);
    if (!line.isEmpty()) {
        ++line_num;
        std::cout << line.getValue() << "\n";
    }

    return;
}

void DataWriter::printOutputUnsorted()
{
    auto line = m_queue.pullNext();

    if (!line.isEmpty()) {
        std::cout << line.getValue() << "\n";
    }

    return;
}

#endif //JM_DATA_WRITER_HPP
