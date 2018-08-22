#include "DataProcessor.hpp"
#include "DataReader.hpp"
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

	// Read input lines
	DataReader& reader = DataReader::read();

	// Process lines
	DataProcessor processor(reader);
	processor.processLines(arg_manager);

	// Print output lines
	std::thread printer = std::thread(&DataProcessor::printOutput, &processor, std::ref(arg_manager));


	// Wait for threads to finish
	printer.join();

	return 0;
}
