#ifndef JM_ARG_MANAGER_HPP
#define JM_ARG_MANAGER_HPP

#include <regex>
#include <unordered_map>
#include <vector>

class ArgManager {
public:
	ArgManager();
	void readArgs(int argc, char **argv);

	bool helpRequested() const;
	void showHelp() const;
	std::string getDelimiter() const;
	std::vector<unsigned> getFields() const;
	std::vector<unsigned> getRegExFields() const;
	bool inverseRegExFields() const;
	std::vector<std::string> splitRegex() const;

private:
	std::vector<unsigned> splitFields(const std::string& arg_val) const;

private:
	enum class State {arg, val};
	enum class Argument {invalid, h, d, f, p, i, x};
	std::unordered_map<std::string, std::string> m_arg_vals;
	std::unordered_map<std::string, Argument>    m_arg_enum;
};

ArgManager::ArgManager()
{
	m_arg_vals["-d"] = " ";
	m_arg_vals["-f"] = "0";
	m_arg_vals["-p"] = "";
	m_arg_vals["-i"] = "0";
	m_arg_vals["-x"] = "";
	m_arg_vals["-h"] = "";

	m_arg_enum["-d"] = Argument::d;
	m_arg_enum["-f"] = Argument::f;
	m_arg_enum["-p"] = Argument::p;
	m_arg_enum["-i"] = Argument::i;
	m_arg_enum["-x"] = Argument::x;
	m_arg_enum["-h"] = Argument::h;

}

void ArgManager::readArgs(int argc, char **argv)
{
	auto state = State::arg;
	std::string option = "";
	std::string error = "";

	for(auto i=1; i<argc; ++i) {
		if (state == State::arg) {
			option = argv[i];

			switch (m_arg_enum[option]) {
				case Argument::d:
				case Argument::f:
				case Argument::p:
				case Argument::x:
					state = State::val;
					break;
				case Argument::i:
					m_arg_vals["-i"] = "1";
					break;
				case Argument::h:
					m_arg_vals["-h"] = "1";
					break;
				default:
					std::cerr << "recut: Invalid option '" + option + "'\n" << std::endl;
					m_arg_vals["-h"] = "1"; // show help on error
					i = argc;               // exit the loop
			}
		} else {
			m_arg_vals[option] = argv[i];
			state = State::arg;
		}
	}

	if (m_arg_vals["-p"] != "" && m_arg_vals["-x"] == "") {
		std::cerr << "recut: Option -p requires option -x.\n" << std::endl;
		m_arg_vals["-h"] = "1"; // show help on error
	} else if (m_arg_vals["-p"] == "" && m_arg_vals["-x"] != "") {
		std::cerr << "recut: Option -x requires option -p.\n" << std::endl;
		m_arg_vals["-h"] = "1"; // show help on error
	} else if (m_arg_vals["-i"] != "" && m_arg_vals["-p"] == "") {
		std::cerr << "recut: Option -i requires option -p.\n" << std::endl;
		m_arg_vals["-h"] = "1"; // show help on error
	}
}

bool ArgManager::helpRequested() const
{
	return (m_arg_vals.at("-h") == "1");
}

void ArgManager::showHelp() const
{
	std::cout << "Usage: recut OPTION... [FILE]...\n" << std::endl;
	std::cout << "With no FILE read standard input.\n" << std::endl;
	std::cout << "Arguments:\n" << std::endl;
	std::cout << "  -h: This help" << std::endl;
	std::cout << "  -d DELIM: use DELIM instead of space for field delimiter." << std::endl;
	std::cout << "  -f FIELDS: comma separated list of fiels to be returned (1-index base)." << std::endl;
	std::cout << "  -p FIELDS: comma separated list of fiels to process with the regex passed in option -x (1-index base)" << std::endl;
	std::cout << "  -x: Regular expression to apply to fields listed in option -p." << std::endl;
	std::cout << "  -i: Invert field's list passed in option -p" << std::endl;
	std::cout << "  -h: This help" << std::endl;
}

std::string ArgManager::getDelimiter() const
{
	return m_arg_vals.at("-d");
}

std::vector<unsigned> ArgManager::getFields() const
{
	auto arg_val = m_arg_vals.at("-f");
	return splitFields(arg_val);
}

std::vector<unsigned> ArgManager::getRegExFields() const
{
	auto arg_val = m_arg_vals.at("-p");
	return splitFields(arg_val);
}

bool ArgManager::inverseRegExFields() const
{
	auto arg_val = m_arg_vals.at("-i");
	return (arg_val == "1");
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

std::vector<std::string> ArgManager::splitRegex() const
{
	auto arg_val = m_arg_vals.at("-x");

	std::regex r("s/(.*)/(.*)/");
	std::smatch pieces;
	std::regex_match(arg_val, pieces, r);
	std::vector<std::string> response;
	response.push_back(pieces[1]);
	response.push_back(pieces[2]);

	return response;
}
#endif //JM_ARG_MANAGER_HPP
