#ifndef JM_INPUT_READER_HPP
#define JM_INPUT_READER_HPP

#include <algorithm>
#include <iostream>
#include <vector>

#include "Line.hpp"

class InputReader {
public:
	static InputReader getInstance();
	std::istream& readLine();
	void processLine(const ArgManager& arg_manager);

private:
	Line m_line;
	std::string m_delimiter;
	std::vector<unsigned> m_fields;

private:
	InputReader() {}
};

InputReader InputReader::getInstance()
{
	static InputReader instance;
	return instance;
}

std::istream& InputReader::readLine()
{
	std::string line;
	auto& stream = std::getline(std::cin, line);
	m_line = Line(line);
	return stream;
}

void InputReader::processLine(const ArgManager& arg_manager)
{
	auto delimiter = arg_manager.getDelimiter();
	auto fields    = arg_manager.getFields();
	auto re_fields = arg_manager.getRegExFields();
	auto regex     = arg_manager.splitRegex();
	auto inverse   = arg_manager.inverseRegExFields();

	std::cout << regex[0] << ":" << regex[1] << std::endl;

	return;
	// split the word
	m_line.split(delimiter);

	// process the required parts, if any
	m_line.process(re_fields, regex[1], inverse);

	// join requested fields
	std::cout << m_line.join(delimiter, fields) << std::endl;
}

#endif //JM_INPUT_READER_HPP
