#ifndef JM_DATA_READER_HPP
#define JM_DATA_READER_HPP

#include "DataQueue.hpp"
#include "Worker.hpp"

class DataReader : public Worker {
public:
    DataReader(DataQueue& queue);
    std::string pull();

private:
    DataQueue& m_queue;

private:
    void do_job();
    DataReader() = delete;
    void readStream();
};

DataReader::DataReader(DataQueue& queue) :
    m_queue(queue)
{
}

void DataReader::do_job()
{
    std::string line;
    while(std::getline(std::cin, line)) {
        m_queue.push(line);
    }
    m_queue.set_eof();
    m_done = true;

    return;
}

#endif //JM_DATA_READER_HPP
