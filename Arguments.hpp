#ifndef JM_ARGUMENTS_HPP
#define JM_ARGUMENTS_HPP

#include <unordered_map>

class Arguments {
private:
    std::unordered_map<std::string, std::string> m_args;

public:
    void set(const std::string& name, const std::string& val);
    std::string get(const std::string& name) const;
};

void Arguments::set(const std::string& name, const std::string& val)
{
    m_args[name] = val;
}

std::string Arguments::get(const std::string& name) const
{
    return m_args.at(name);
}

#endif //JM_ARGUMENTS_HPP
