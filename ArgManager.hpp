#ifndef JM_ARG_MANAGER_HPP
#define JM_ARG_MANAGER_HPP

#include <iostream>
#include <regex>
#include <sys/stat.h>
#include <vector>

#include "Arguments.hpp"

class ArgManager {
public:
    ArgManager();
    bool processArgs(int argc, char **argv);
    Arguments getArgs() const;
    void showHelp() const;
    bool isHelpRequested() const;

private:
    Arguments m_args;
    bool m_status_ok = true;
    enum class State {inv, arg, val, file};
    const std::vector<std::string> m_unary = {"-h", "-i", "-s"};
    const std::vector<std::string> m_binary = {"-d", "-f", "-p", "-x"};
    void addFile(const std::string& file_name);
    bool is_file(const std::string& path) const;
    bool is_dir (const std::string& path) const;

private:
    bool isUnaryArgument(const std::string& option) const;
    bool isBinaryArgument(const std::string& option) const;
    void flagError(const std::string& msg);
    void validate();
    bool validateList(const std::string& list) const;
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
                addFile(option);
                state = State::file;
            }

        } else if (state == State::val) {
            value = argv[i];
            if (option == "-x") {
                auto regex = splitRegex(value);
                m_args.set("-xs", regex[0]);
                m_args.set("-xr", regex[1]);
            }
            m_args.set(option, value);
            state = State::arg;
        } else if (state == State::file) {
            addFile(argv[i]);
        } else {
            flagError("Cannot read arguments: unexpected value.");
            i = argc;
        }
    }

    if (m_status_ok && state == State::val) {
        flagError("Expected value for option '" + option + "'");
    }

    validate();

    return m_status_ok;
}

void ArgManager::addFile(const std::string& file_name)
{
    static auto file_count = 0u;
    auto name = "file" + std::to_string(file_count++);
    m_args.set(name, file_name);
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
    if (m_args.get("-d") == "") {
        flagError("Option -d does not accept empty value.");
    } else if (!validateList(m_args.get("-f"))) {
        flagError("Option -f expects a comma separated list of integers");
    } else if (!validateList(m_args.get("-p"))) {
        flagError("Option -p expects a comma separated list of integers");
    } else if (m_args.get("-x") != "" && m_args.get("-xs") == "") {
        flagError("Search pattern '" + m_args.get("-x") + "' in option -x cannot be empty.");
    } else if (m_args.get("-i") == "1" && m_args.get("-p") == "") {
        flagError("Option -i requires option -p with non-empty value.");
    } else if (m_args.get("-p") != "" && m_args.get("-x") == "") {
        flagError("Option -p requires option -x with non-empty value.");
    } else {
        auto file_names = m_args.find_all_matching("file");
        for (const auto& file_name : file_names) {
            if (!is_file(file_name)) {
                flagError("Cannot open file " + file_name + " for reading.");
                break;
            }
        }
    }
}

bool ArgManager::is_file(const std::string& path) const
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return S_ISREG(buf.st_mode);
}

bool ArgManager::is_dir(const std::string& path) const
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return S_ISDIR(buf.st_mode);
}

bool ArgManager::validateList(const std::string& list) const
{
    auto regex    = std::regex("^[1-9]\\d*(,[1-9]\\d*)*$");
    return (list == "" || std::regex_match(list, regex));
}

bool ArgManager::isHelpRequested() const
{
    auto is_requested = false;

    if (!m_status_ok) {
        is_requested = true;
    } else if (m_args.get("-h") == "1") {
        is_requested = true;
    }

    return is_requested;
}

void ArgManager::showHelp() const
{
    std::ostream& out = m_status_ok ? std::cout : std::cerr;

    out << "Usage: xcut OPTION... [FILE]...\n";
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
