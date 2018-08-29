#ifndef JM_LINE_HPP
#define JM_LINE_HPP

#include <regex>
#include <string>
#include <vector>
#include <iostream>

#include "ArgManager.hpp"

typedef std::string str;
typedef std::vector<unsigned> uvec;

class Line {
public:
    Line() {}
    Line(const str& line);
    void split(const str& delimiter);
    std::string process(const ArgManager& arg_manager);
    str join(const str& delimiter, const uvec& fields);

private:
    unsigned m_line_num;
    str m_line;
    str m_joined;
    std::vector<str> m_parts;

private:
    void joinList(const str& delimiter, const uvec& fields);
    void joinAll(const str& delimiter);
    unsigned getNumParts() const;
    void processPart(int part_num, const str& re_search, const str& re_replace);
    void processPart(int part_num, const str& regex);
    bool find(unsigned needle, const uvec& haystack) const;
};

Line::Line(const str& line) : m_line(line)
{
    static auto line_num = 0;
    m_line_num = ++line_num;
}

void Line::split(const str& delimiter)
{
    auto pos_start = 0u;
    auto pos_end = str::npos;

    while((pos_end = m_line.find(delimiter,pos_start)) != str::npos) {
        m_parts.push_back(m_line.substr(pos_start, (pos_end-pos_start)));
        pos_start = pos_end + 1;
    }
    m_parts.push_back(m_line.substr(pos_start, (pos_end-pos_start)));
}

unsigned Line::getNumParts() const
{
    return m_parts.size();
}

std::string Line::process(const ArgManager& arg_manager)
{
    auto delimiter  = arg_manager.getDelimiter();
    auto fields     = arg_manager.getFields();
    auto re_fields  = arg_manager.getRegExFields();
    auto re_search  = arg_manager.getRegExSearch();
    auto re_replace = arg_manager.getRegExReplace();
    auto inverse    = arg_manager.getInverseRegExFields();

    // split the word
    split(delimiter);

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
            processPart(i, re_search, re_replace);
        }
    }

    // join requested fields and save string
    return join(delimiter, fields);
}

bool Line::find(unsigned needle, const uvec& haystack) const
{
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}

void Line::processPart(int part_num, const str& re_search, const str& re_replace)
{
    try {
        static const std::regex r(re_search);
        m_parts[part_num] = std::regex_replace(m_parts[part_num], r, re_replace);
    } catch (...) {}
}


str Line::join(const str& delimiter, const uvec& fields)
{

    if (fields.size() == 0) {
        joinAll(delimiter);
    } else {
        joinList(delimiter, fields);
    }
    return m_joined;
}

void Line::joinList(const str& delimiter, const uvec& fields)
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

void Line::joinAll(const str& delimiter)
{
    for (auto i = 0u; i<getNumParts(); ++i) {
        m_joined += (i == 0) ? "" : delimiter;
        m_joined += m_parts[i];
    }
}

#endif //JM_LINE_HPP
