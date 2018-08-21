#ifndef JM_INPUT_READER_HPP
#define JM_INPUT_READER_HPP

#include <algorithm>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "Line.hpp"

class DataProcessor {
public:
	static DataProcessor& getInstance();
	void readLines();
	void processLines(const ArgManager& arg_manager);
	void printOutput(const ArgManager& arg_manager);

private:
	std::queue<std::string> m_lines_in;
	std::unordered_map<unsigned, std::string> m_lines_out;
	std::mutex m_mtx_lines_in;
	std::mutex m_mtx_lines_out;
	bool m_still_reading = true;
	bool m_still_processing = true;

private:
	void processLine(unsigned line_num, std::string src_line, const ArgManager& arg_manager);
	void enqueueInput(const std::string& input);
	void enqueueOutput(unsigned line_num, const std::string& output);
	std::string dequeueInput();
	std::string dequeueOutput();
	std::string getOutputLine(unsigned line_num);
	void printOutputSorted();
	void printOutputUnsorted();
	unsigned countLines(unsigned line_num);

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
	auto line_num = 0u;
	std::vector<std::thread> threads;

	// If stopped reading we may still have some items to process
	while (m_still_reading || m_lines_in.size() > 0) {
		if (m_lines_in.size() > 0) {
			auto val = dequeueInput();
			threads.push_back(std::thread(&DataProcessor::processLine, this, line_num++, val, arg_manager));
		}
	}

	// Wait for all threads to finish.
	for (auto& t : threads) {
		t.join();
	}

	m_still_processing = false;

	return;
}

void DataProcessor::processLine(unsigned line_num, std::string src_line, const ArgManager& arg_manager)
{
	Line line(src_line);
	enqueueOutput(line_num, line.process(arg_manager));

	return;
}

void DataProcessor::printOutput(const ArgManager& arg_manager)
{
	while (m_still_processing || m_lines_out.size() > 0) {
		if (arg_manager.sortOutput()) {
			printOutputSorted();
		} else {
			printOutputUnsorted();
		}
	}

	return;
}

void DataProcessor::printOutputSorted()
{
	static auto line_num = 0u;
	auto count = countLines(line_num);
	if (count > 0) {
		auto val = getOutputLine(line_num);
		++line_num;
		std::cout << val << "\n";
	}

	return;
}

unsigned DataProcessor::countLines(unsigned line_num)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_out);
	return m_lines_out.count(line_num);
}

void DataProcessor::printOutputUnsorted()
{
	if (m_lines_out.size() > 0) {
		auto val = dequeueOutput();
		std::cout << val << "\n";
	}

	return;
}

void DataProcessor::enqueueInput(const std::string& input)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_in);
	m_lines_in.push(input);
	return;
}

void DataProcessor::enqueueOutput(unsigned line_num, const std::string& output)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_out);
	m_lines_out[line_num] = output;
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
	auto it = m_lines_out.begin();
	auto val = it->second;
	m_lines_out.erase(m_lines_out.begin());
	return val;
}

std::string DataProcessor::getOutputLine(unsigned line_num)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_out);
	auto val = m_lines_out.at(line_num);
	m_lines_out.erase(line_num);
	return val;
}

#endif //JM_INPUT_READER_HPP
