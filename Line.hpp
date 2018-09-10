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
    std::string process(const Arguments& args);
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
    void split(const str& delimiter);
    std::vector<unsigned> splitFields(const std::string& arg_val) const;
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

std::string Line::process(const Arguments& args)
{
    static const auto fields     = splitFields(args.get("-f"));
    static const auto re_fields  = splitFields(args.get("-p"));
    static const auto delimiter  = args.get("-d");
    static const auto inverse    = args.get("-i");
    static const auto re_search  = args.get("-xs");
    static const auto re_replace = args.get("-xr");

    // split the word
    split(delimiter);

    for (auto i = 0u; i<getNumParts(); ++i) {
        bool process = false;

        if (re_fields.size() == 0) {
            process = true;
        } else if (find(i+1, re_fields)) {
            process = (inverse == "0");
        } else {
            process = (inverse == "1");
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

std::vector<unsigned> Line::splitFields(const std::string& s) const
{
    std::vector<unsigned> fields;

    auto regex    = std::regex(",");
    auto begin = std::sregex_token_iterator(s.begin(), s.end(), regex, -1);
    auto end   = std::sregex_token_iterator();
    std::for_each(begin, end, [&](const std::string& m) {
        if (!m.empty()) {
            fields.push_back(std::stoul(m));
        }
    });

    return fields;
}

#endif //JM_LINE_HPP
