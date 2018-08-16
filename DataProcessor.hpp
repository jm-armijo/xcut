#ifndef JM_INPUT_READER_HPP
#define JM_INPUT_READER_HPP

#include <algorithm>
#include <iostream>
#include <vector>

#include "Line.hpp"

class DataProcessor {
public:
	static DataProcessor getInstance();
	std::istream& readLine();
	void processLine(const ArgManager& arg_manager);

private:
	Line m_line;
	std::string m_delimiter;
	std::vector<unsigned> m_fields;

private:
	DataProcessor() {}
};

DataProcessor DataProcessor::getInstance()
{
	static DataProcessor instance;
	return instance;
}

std::istream& DataProcessor::readLine()
{
	std::string line;
	auto& stream = std::getline(std::cin, line);
	m_line = Line(line);
	return stream;
}

void DataProcessor::processLine(const ArgManager& arg_manager)
{
	auto delimiter  = arg_manager.getDelimiter();
	auto fields     = arg_manager.getFields();
	auto re_fields  = arg_manager.getRegExFields();
	auto re_search  = arg_manager.getRegExSearch();
	auto re_replace = arg_manager.getRegExReplace();
	auto inverse    = arg_manager.getInverseRegExFields();

	// split the word
	m_line.split(delimiter);

	// process the required parts, if any
	m_line.process(re_fields, re_search, re_replace, inverse);

	// join requested fields
	std::cout << m_line.join(delimiter, fields) << std::endl;
}

#endif //JM_INPUT_READER_HPP
