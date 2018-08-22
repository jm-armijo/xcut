#ifndef JM_DATA_READER_HPP
#define JM_DATA_READER_HPP

#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class DataReader {
public:
	static DataReader& read();
	std::string get();
	bool done() const;
	unsigned size() const;
	~DataReader();

private:
	std::thread m_thread;
	std::queue<std::string> m_lines;
	std::mutex m_mtx_lines;
	bool m_still_reading = true;

private:
	DataReader();
	void push(const std::string& input);
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
	m_thread.join();
}

bool DataReader::done() const
{
	return !m_still_reading;
}

unsigned DataReader::size() const
{
	return m_lines.size();
}

void DataReader::readStream()
{
	std::string line;
	while(std::getline(std::cin, line)) {
		push(line);
	}
	m_still_reading = false;

	return;
}

void DataReader::push(const std::string& input)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines);
	m_lines.push(input);
	return;
}

std::string DataReader::get()
{
	std::lock_guard<std::mutex> guard(m_mtx_lines);
	auto val = m_lines.front();
	m_lines.pop();
	return val;
}

#endif //JM_DATA_READER_HPP
