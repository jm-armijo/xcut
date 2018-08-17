#ifndef JM_INPUT_READER_HPP
#define JM_INPUT_READER_HPP

#include <algorithm>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "Line.hpp"

class DataProcessor {
public:
	static DataProcessor& getInstance();
	void readLines();
	void processLines(const ArgManager& arg_manager);
	void printOutput();

private:
	std::queue<std::string> m_lines_in;
	std::queue<std::string> m_lines_out;
	std::mutex m_mtx_lines_in;
	std::mutex m_mtx_lines_out;
	bool m_still_reading = true;
	bool m_still_processing = true;

private:
	void enqueueInput(const std::string& input);
	void enqueueOutput(const std::string& output);
	std::string dequeueInput();
	std::string dequeueOutput();
	void processLine(std::string src_line, const ArgManager& arg_manager);

private:
	DataProcessor() {}
};

DataProcessor& DataProcessor::getInstance()
{
	static DataProcessor instance;
	return instance;
}

void DataProcessor::readLines()
{
	std::string line;
	while(std::getline(std::cin, line)) {
		enqueueInput(line);
	}
	m_still_reading = false;

	return;
}

void DataProcessor::processLines(const ArgManager& arg_manager)
{
	std::vector<std::thread> threads;

	// Try at least once in case we stopped reading before reaching this point.
	do {
		while (m_lines_in.size() > 0) {
			auto val = dequeueInput();
			threads.push_back(std::thread(&DataProcessor::processLine, this, val, arg_manager));
		}
	} while (m_still_reading || m_lines_in.size() > 0);
	// If stopped reading we may still have some items to process

	// Wait for all threads to finish.
	for (auto& t : threads) {
		t.join();
	}

	m_still_processing = false;

	return;
}

void DataProcessor::processLine(std::string src_line, const ArgManager& arg_manager)
{
	Line line(src_line);
	enqueueOutput(line.process(arg_manager));

	return;
}

void DataProcessor::printOutput()
{
	do {
		while (m_lines_out.size() > 0) {
			auto val = dequeueOutput();
			std::cout << val << "\n";
		}
	} while (m_still_processing|| m_lines_out.size() > 0);

	return;
}

void DataProcessor::enqueueInput(const std::string& input)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_in);
	m_lines_in.push(input);
	return;
}

void DataProcessor::enqueueOutput(const std::string& output)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_out);
	m_lines_out.push(output);
	return;
}

std::string DataProcessor::dequeueInput()
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_in);
	auto val = m_lines_in.front();
	m_lines_in.pop();
	return val;
}

std::string DataProcessor::dequeueOutput()
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_out);
	auto val = m_lines_out.front();
	m_lines_out.pop();
	return val;
}

#endif //JM_INPUT_READER_HPP
