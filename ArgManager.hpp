#ifndef JM_ARG_MANAGER_HPP
#define JM_ARG_MANAGER_HPP

#include <iostream>
#include <regex>
#include <tuple>
#include <unordered_map>
#include <vector>

enum class State {inv, arg, val};
enum class Argument {h, d, f, p, i, x, s};

class ArgManager {
public:
	// Entry point to start reading and processing arguments
	void readArgs(int argc, char **argv);

	// Getter functions
	bool getHelp() const;
	void printHelp() const;
	std::string getDelimiter() const;
	std::string getRegExSearch() const;
	std::string getRegExReplace() const;
	std::vector<unsigned> getRegExFields() const;
	std::vector<unsigned> getFields() const;
	bool getInverseRegExFields() const;
	bool sortOutput() const {return m_sort;}

private:
	// Stores the overal status of the argument reading operation
	bool m_read_ok = true;

	// Options storage
	bool m_help = false;
	bool m_re_invert_fields = false;
	bool m_sort = false;
	std::string m_delimiter = " ";
	std::string m_regex;
	std::string m_re_search;
	std::string m_re_replace;
	std::vector<unsigned> m_re_fields;
	std::vector<unsigned> m_fields;

private:
	// Methods to read arguments
	bool isUnaryOption(const std::string& option) const;
	bool isBinaryOption(const std::string& option) const;
	void setValue(const std::string& option, const std::string& value);
	void flagError(const std::string& msg);
	void validateArgs();

	// Methods to process arguments
	std::vector<unsigned> splitFields(const std::string& arg_val) const;
	std::tuple<std::string, std::string> splitRegex(const std::string& arg_val) const;

};

void ArgManager::readArgs(int argc, char **argv)
{
	auto state = State::arg;
	std::string option = "";

	for(auto i=1; i<argc; ++i) {
		if (state == State::arg) {

			option = argv[i];
			if (isUnaryOption(option)) {
				setValue(option, "1");
			} else if (isBinaryOption(option)) {
				state = State::val;
			} else {
				flagError("Invalid option '" + option + "'");
				i = argc; // exit the loop
			}

		} else {
			setValue(option, argv[i]);
			state = State::arg;
		}
	}

	if (m_read_ok && state == State::val) {
		flagError("Expected value for option '" + option + "'");
	} else if (m_read_ok) {
		validateArgs();
	}
}

bool ArgManager::isUnaryOption(const std::string& option) const
{
	static const std::vector<std::string> unary = {"-h", "-i", "-s"};
	return (std::find(unary.begin(), unary.end(), option) != unary.end());
}

bool ArgManager::isBinaryOption(const std::string& option) const
{
	static const std::vector<std::string> binary = {"-d", "-f", "-p", "-x"};
	return (std::find(binary.begin(), binary.end(), option) != binary.end());
}

void ArgManager::setValue(const std::string& option, const std::string& value)
{
	if (option == "-h") {
		m_help = (value == "1");
	} else if (option == "-i") {
		m_re_invert_fields = (value == "1");
	} else if (option == "-s") {
		m_sort = (value == "1");
	} else if (option == "-d") {
		m_delimiter = value;
	} else if (option == "-f") {
		m_fields = splitFields(value);
	} else if (option == "-p") {
		m_re_fields = splitFields(value);
	} else if (option == "-x") {
		m_regex = value;
		std::tie(m_re_search, m_re_replace) = splitRegex(value);
	} else {
		std::runtime_error("Unhandled option '" + option + "'");
	}
}

void ArgManager::flagError(const std::string& msg)
{
	std::cerr << "xcut: " << msg << "\n" << std::endl;
	m_read_ok = false;
}

void ArgManager::validateArgs()
{
	if (m_re_fields.size() > 0 && m_regex == "") {
		flagError("Option -p requires option -x.");
	} else if (m_re_invert_fields && m_re_fields.size() == 0) {
		flagError("Option -i requires option -p.");
	} else if (m_regex != "" && m_re_search == "") {
		flagError("Option -x requires a valid non-empty search pattern.");
	}
}


// Getter functions
bool ArgManager::getHelp() const
{
	return (m_help || !m_read_ok);
}

void ArgManager::printHelp() const
{
	std::ostream& out = m_help ? std::cout : std::cerr;

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

std::string ArgManager::getDelimiter() const
{
	return m_delimiter;
}

std::vector<unsigned> ArgManager::getFields() const
{
	return m_fields;
}

std::vector<unsigned> ArgManager::getRegExFields() const
{
	return m_re_fields;
}

std::string ArgManager::getRegExSearch() const
{
	return m_re_search;
}

std::string ArgManager::getRegExReplace() const
{
	return m_re_replace;
}

bool ArgManager::getInverseRegExFields() const
{
	return m_re_invert_fields;
}

std::vector<unsigned> ArgManager::splitFields(const std::string& arg_val) const
{
	auto pos_start = 0u;
	auto pos_end   = std::string::npos;

	std::vector<unsigned> fields;
	while ((pos_end = arg_val.find(",", pos_start)) != std::string::npos) {
		auto val = std::stoul(arg_val.substr(pos_start, (pos_end-pos_start)));
		fields.push_back(val);
		pos_start = pos_end+1;
	}
	fields.push_back(std::stoi(arg_val.substr(pos_start, (pos_end-pos_start))));
	
	return fields;
}

std::tuple<std::string, std::string> ArgManager::splitRegex(const std::string& arg_val) const
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

	return std::make_tuple(search, replace);
}

#endif //JM_ARG_MANAGER_HPP
