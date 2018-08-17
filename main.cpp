#include "DataProcessor.hpp"
#include "ArgManager.hpp"
#include <stdexcept>
#include <thread>


int main(int argc, char **argv)
{
	ArgManager arg_manager;
	arg_manager.readArgs(argc, argv);

	if (arg_manager.getHelp()) {
		arg_manager.printHelp();
		return 1;
	}

	DataProcessor& processor = DataProcessor::getInstance();

	// Read input lines
	std::thread reader = std::thread(&DataProcessor::readLines, &processor);

	// Print output lines
	std::thread printer = std::thread(&DataProcessor::printOutput, &processor);

	// Process lines
	processor.processLines(arg_manager);

	// Wait for threads to finish
	reader.join();
	printer.join();


	return 0;
}
