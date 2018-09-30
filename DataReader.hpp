#ifndef JM_DATA_READER_HPP
#define JM_DATA_READER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "DataQueue.hpp"
#include "Worker.hpp"

class DataReader : public Worker {
public:
    DataReader(const Arguments& args, DataQueue& queue);

private:
    DataQueue& m_queue;
    std::vector<std::string> m_files;

private:
    void doJob();
    DataReader() = delete;
    void readFromStream(std::istream& in);
};

DataReader::DataReader(const Arguments& args, DataQueue& queue) :
    Worker(args), m_queue(queue)
{
    m_files = m_args.find_all_matching("file");
}

void DataReader::doJob()
{
    if (m_files.empty()) {
        std::istream& in = std::cin;
        readFromStream(in);
    } else {
        for (auto& file : m_files) {
            std::ifstream in (file, std::ifstream::in);
            readFromStream(in);
            in.close();
        }
    }
    m_done = true;
}

void DataReader::readFromStream(std::istream& in)
{
    auto line_value = std::string();
    while(std::getline(in, line_value)) {
        auto line = Line(line_value);
        m_queue.push(Line(line));
    }

    return;
}

#endif //JM_DATA_READER_HPP
