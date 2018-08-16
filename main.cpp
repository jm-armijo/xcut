#include "DataProcessor.hpp"
#include "ArgManager.hpp"
#include <stdexcept>


int main(int argc, char **argv)
{
	ArgManager arg_manager;
	arg_manager.readArgs(argc, argv);

	if (arg_manager.getHelp()) {
		arg_manager.printHelp();
		return 1;
	}

	DataProcessor processor = DataProcessor::getInstance();
	while (processor.readLine()) {
		processor.processLine(arg_manager);
	}

	return 0;
}
