#ifndef JM_DATA_READER_HPP
#define JM_DATA_READER_HPP

#include <iostream>
#include <thread>

#include "LineQueue.hpp"

class DataReader {
public:
    static DataReader& read();
    std::string pull();
    bool done() const;
    bool size() const;
    ~DataReader();

private:
    LineQueue m_queue;
    std::thread m_thread;
    bool m_done = false;

private:
    DataReader();
    void readStream();
};

DataReader& DataReader::read()
{
    static DataReader instance;
    return instance;
}

DataReader::DataReader()
{
	m_thread = std::thread(&DataReader::readStream, this);
}

DataReader::~DataReader()
{
    // Wait until all input is read
    while(!done());

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void DataReader::readStream()
{
    std::string line;
    while(std::getline(std::cin, line)) {
        m_queue.push(line);
    }
    m_done = true;

    return;
}

bool DataReader::done() const
{
    return m_done;
}

bool DataReader::size() const
{
    return m_queue.size();
}

std::string DataReader::pull()
{
    return m_queue.pull();
}

#endif //JM_DATA_READER_HPP
