#ifndef JM_DATA_READER_HPP
#define JM_DATA_READER_HPP

#include <atomic>
#include <thread>
#include "DataQueue.hpp"

class DataReader {
public:
    DataReader(DataQueue& queue);
    void do_job();
	bool done() const;
    std::string pull();
    ~DataReader();

private:
    DataQueue& m_queue;
    std::thread m_thread;
	std::atomic<bool> m_done{false};

private:
    DataReader() = delete;
    void readStream();
};

DataReader::DataReader(DataQueue& queue) :
    m_queue(queue)
{
}

void DataReader::do_job()
{
    m_thread = std::thread(&DataReader::readStream, this);
}

bool DataReader::done() const
{
	return m_done;
}

void DataReader::readStream()
{
    std::string line;
    while(std::getline(std::cin, line)) {
        m_queue.push(line);
    }
    m_queue.set_eof();
	m_done = true;

    return;
}

DataReader::~DataReader()
{
    // Wait until all input is read
    while(true) {
        if (m_queue.is_eof()) {
            break;
        }
    }

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

#endif //JM_DATA_READER_HPP
