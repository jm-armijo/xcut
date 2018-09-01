#ifndef JM_ARG_MANAGER_HPP
#define JM_ARG_MANAGER_HPP

#include <iostream>
#include <regex>
#include <vector>

#include "Arguments.hpp"

class ArgManager {
public:
    ArgManager();
    bool processArgs(int argc, char **argv);
    Arguments getArgs() const;
    void printHelp() const;

private:
    Arguments m_args;
    bool m_status_ok = true;
    enum class State {inv, arg, val};
    const std::vector<std::string> m_unary = {"-h", "-i", "-s"};
    const std::vector<std::string> m_binary = {"-d", "-f", "-p", "-x"};

private:
    bool isUnaryArgument(const std::string& option) const;
    bool isBinaryArgument(const std::string& option) const;
    void flagError(const std::string& msg);
    void validate();
    std::vector<std::string> splitRegex(const std::string& arg_val) const;
};

ArgManager::ArgManager()
{
    // Set default arg values
    m_args.set("-h", "0");
    m_args.set("-i", "0");
    m_args.set("-s", "0");
    m_args.set("-d", " ");
    m_args.set("-f", "");
    m_args.set("-p", "");
    m_args.set("-x", "");
}

bool ArgManager::processArgs(int argc, char **argv)
{
    auto state = State::arg;
    auto option = std::string();
    auto value = std::string();

    for(auto i=1; i<argc; ++i) {
        if (state == State::arg) {

            option = argv[i];
            if (isUnaryArgument(option)) {
                m_args.set(option, "1");
            } else if (isBinaryArgument(option)) {
                state = State::val;
            } else {
                flagError("Invalid option '" + option + "'");
                i = argc; // exit the loop
            }

        } else {
            value = argv[i];
            if (option == "-x") {
                auto regex = splitRegex(value);
                m_args.set("-xs", regex[0]);
                m_args.set("-xr", regex[1]);
            }
            m_args.set(option, value);
            state = State::arg;
        }
    }

    if (m_status_ok && state == State::val) {
        flagError("Expected value for option '" + option + "'");
    }

    validate();

    return m_status_ok;
}

Arguments ArgManager::getArgs() const
{
    return m_args;
}

bool ArgManager::isUnaryArgument(const std::string& option) const
{
    return (std::find(m_unary.begin(), m_unary.end(), option) != m_unary.end());
}

bool ArgManager::isBinaryArgument(const std::string& option) const
{
    return (std::find(m_binary.begin(), m_binary.end(), option) != m_binary.end());
}

void ArgManager::flagError(const std::string& msg)
{
    std::cerr << "xcut: " << msg << "\n" << std::endl;
    m_status_ok = false;
}

void ArgManager::validate()
{
    for (const auto& arg : m_binary) {
        if (m_status_ok) {
            auto value = m_args.get(arg);
            if (arg == "-d") {
                if (value == "") {
                    flagError("Option " + arg + " does not accept empty value.");
                }
            } else if (arg == "-f" || arg == "-p") {
                if (value != "" && !std::regex_match (value, std::regex("^[1-9]\\d*(,[1-9]\\d*)*$") )) {
                    flagError("Option " + arg + "expects a comma separated list of integers");
                }
            } else if (arg == "-x") {
                if (value != "" && m_args.get("-xs") == "") {
                    flagError("Search pattern '" + value + "' in option '" + arg + "' cannot be empty.");
                }
            }
        }
    }

    if (m_status_ok) {
        if (m_args.get("-i") == "1" && m_args.get("-p") == "") {
            flagError("Option -i requires option -p with non-empty value.");
        } else if (m_args.get("-p") != "" && m_args.get("-x") == "") {
            flagError("Option -p requires option -x with non-empty value.");
        }
    }
}


void ArgManager::printHelp() const
{
    std::ostream& out = m_args.get("-h") == "1" ? std::cout : std::cerr;

    out << "Usage: xcut OPTION... < [FILE]...\n";
    out << "From each FILE, replaces text on all or selected parts of lines using PATTERN,\n";
    out << "and print all or selected parts to standard output.\n\n";

    out << "Example: xcut -f 1,2 -x 's/\\d/<num>/' < file.txt\n\n";

    out << "Options\n";
    out << "  -d DELIM    Use DELIM instead of SPACE for field delimiter.\n";
    out << "  -f FIELDS   Comma separated list of fiels to print (1-index base).\n";
    out << "  -p FIELDS   Comma separated list of fiels to apply PATTERN to. (1-index base)\n";
    out << "  -x PATTERN  sed like Regular Expression to be applied on all or specified parts.\n";
    out << "  -i          Apply PATTERN to inversed -p list\n";
    out << "  -s          Output lines sorted in the original order.\n";
    out << "  -h          This help\n";

    out << "\nAll options are optional, except in these cases:\n";
    out << "    If option -p is used, option -x becomes mandatory\n";
    out << "    If option -i is used, option -x becomes mandatory\n";
    out << std::flush;
}


std::vector<std::string> ArgManager::splitRegex(const std::string& arg_val) const
{
    static const std::regex match_re("s/(.*)/(.*)/");

    std::string search;
    std::string replace;

    std::smatch parts;
    if (std::regex_match(arg_val, parts, match_re)) {
        search = parts[1];

        // Properly escape sequence \/
        std::regex clean_slash("\\\\(/)");
        search = std::regex_replace(search, clean_slash, "$01");

        // Properly escape double backslash sequence (\\)
        std::regex clean_backslash("\\\\\\\\");
        search = std::regex_replace(search, clean_backslash, "\\\\\\\\");

        replace = parts[2];
    }

    return std::vector<std::string>({search, replace});
}

#endif //JM_ARG_MANAGER_HPP
