#ifndef JM_LINE_HPP
#define JM_LINE_HPP

#include <regex>
#include <string>
#include <vector>
#include <iostream>

#include "ArgManager.hpp"

class Line {
public:
	Line() {}
	Line(const std::string& line);
	void split(const std::string& delimiter);
	void process(const std::vector<unsigned>& re_fields, const std::string& regex, bool inverse);
	std::string join(const std::string& delimiter, const std::vector<unsigned>& fields);

private:
	unsigned m_line_num;
	std::string m_line;
	std::string m_joined;
	std::vector<std::string> m_parts;

private:
	void joinList(const std::string& delimiter, const std::vector<unsigned>& fields);
	void joinAll(const std::string& delimiter);
	unsigned getNumParts() const;
	void processPart(int part_num, const std::string& regex);
	bool find(unsigned needle, const std::vector<unsigned>& haystack) const;
};

Line::Line(const std::string& line) : m_line(line)
{
	static auto line_num = 0;
	m_line_num = ++line_num;
}

void Line::split(const std::string& delimiter)
{
	auto pos_start = 0u;
	auto pos_end = std::string::npos;

	while((pos_end = m_line.find(delimiter,pos_start)) != std::string::npos) {
		m_parts.push_back(m_line.substr(pos_start, (pos_end-pos_start)));
		pos_start = pos_end + 1;
	}
	m_parts.push_back(m_line.substr(pos_start, (pos_end-pos_start)));
}

unsigned Line::getNumParts() const
{
	return m_parts.size();
}

void Line::process(const std::vector<unsigned>& re_fields, const std::string& regex, bool inverse)
{
	for (auto i = 0u; i<getNumParts(); ++i) {
		bool process = false;

		if (re_fields.size() == 0) {
			process = true;
		} else if (find(i+1, re_fields)) {
			process = !inverse;
		} else {
			process = inverse;
		}

		if (process) {
			processPart(i, regex);
		}
	}
}

bool Line::find(unsigned needle, const std::vector<unsigned>& haystack) const
{
	return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}

void Line::processPart(int part_num, const std::string& regex)
{
	std::regex r(regex);
	m_parts[part_num] = std::regex_replace(m_parts[part_num], r, "X");
}


std::string Line::join(const std::string& delimiter, const std::vector<unsigned>& fields)
{

	if (fields.size() == 0) {
		joinAll(delimiter);
	} else {
		joinList(delimiter, fields);
	}
	return m_joined;
}

void Line::joinList(const std::string& delimiter, const std::vector<unsigned>& fields)
{
	bool first = true;
	for (auto i : fields) {
		if (i<=getNumParts()) {
			m_joined += first ? "" : delimiter;
			m_joined += m_parts[i-1];
			first = false;
		}
	}
}

void Line::joinAll(const std::string& delimiter)
{
	for (auto i = 0u; i<getNumParts(); ++i) {
		m_joined += (i == 0) ? "" : delimiter;
		m_joined += m_parts[i];
	}
}

#endif //JM_LINE_HPP
