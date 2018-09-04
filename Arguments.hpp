#ifndef JM_ARGUMENTS_HPP
#define JM_ARGUMENTS_HPP

#include <unordered_map>
#include <vector>

class Arguments {
private:
    std::unordered_map<std::string, std::string> m_args;

public:
    void set(const std::string& name, const std::string& val);
    std::string get(const std::string& name) const;
    std::vector<std::string> find_all_matching(const std::string& pattern) const;
    bool exists(const std::string& name) const;
};

void Arguments::set(const std::string& name, const std::string& val)
{
    m_args[name] = val;
}

std::string Arguments::get(const std::string& name) const
{
    return m_args.at(name);
}

std::vector<std::string> Arguments::find_all_matching(const std::string& pattern) const
{
    std::vector<std::string> keys;
    std::for_each(m_args.begin(), m_args.end(), [&](std::pair<std::string, std::string> e){
        if (std::regex_match (e.first, std::regex("^"+pattern+".*") )) {
            keys.push_back(e.second);
        }
    });

    return keys;
}

bool Arguments::exists(const std::string& name) const
{
    return m_args.count(name) > 0;
}

#endif //JM_ARGUMENTS_HPP
