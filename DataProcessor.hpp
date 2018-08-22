#ifndef JM_INPUT_READER_HPP
#define JM_INPUT_READER_HPP

#include <algorithm>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "DataReader.hpp"
#include "Line.hpp"

class DataProcessor {
public:
	DataProcessor(DataReader& reader);
	void processLines(const ArgManager& arg_manager);
	void printOutput(const ArgManager& arg_manager);

private:
	DataReader& m_reader;
	std::unordered_map<unsigned, std::string> m_lines_out;
	std::mutex m_mtx_lines_out;
	bool m_still_processing = true;

private:
	void processLine(unsigned line_num, std::string src_line, const ArgManager& arg_manager);
	void enqueueOutput(unsigned line_num, const std::string& output);
	std::string dequeueOutput();
	std::string getOutputLine(unsigned line_num);
	void printOutputSorted();
	void printOutputUnsorted();
	unsigned countLines(unsigned line_num);
};

DataProcessor::DataProcessor(DataReader& reader) :
	m_reader(reader)
{
}

void DataProcessor::processLines(const ArgManager& arg_manager)
{
	auto line_num = 0u;
	std::vector<std::thread> threads;

	// If stopped reading we may still have some items to process
	while (!m_reader.done() || m_reader.size() > 0) {
		if (m_reader.size() > 0) {
			auto val = m_reader.get();
			threads.push_back(std::thread(&DataProcessor::processLine, this, line_num++, val, std::ref(arg_manager)));
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

void DataProcessor::enqueueOutput(unsigned line_num, const std::string& output)
{
	std::lock_guard<std::mutex> guard(m_mtx_lines_out);
	m_lines_out[line_num] = output;
	return;
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
