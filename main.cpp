#include "InputReader.hpp"
#include "ArgManager.hpp"
#include <stdexcept>


int main(int argc, char **argv)
{
	ArgManager arg_manager;
	arg_manager.readArgs(argc, argv);

	if (arg_manager.helpRequested()) {
		arg_manager.showHelp();
		return 1;
	}

	InputReader reader = InputReader::getInstance();
	while (reader.readLine()) {
		reader.processLine(arg_manager);
	}

	return 0;
}
