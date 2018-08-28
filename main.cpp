#include "ArgManager.hpp"
#include "DataProcessor.hpp"
#include "DataQueue.hpp"
#include "DataReader.hpp"
#include "DataWriter.hpp"

int main(int argc, char **argv)
{
    DataQueue queue_in;
    DataQueue queue_out;

    ArgManager arg_manager;
    arg_manager.readArgs(argc, argv);

    if (arg_manager.getHelp()) {
        arg_manager.printHelp();
        return 1;
    }

    // Read input lines
    DataReader reader(queue_in);
    reader.do_job();

    // Process lines
    DataProcessor processor(queue_in, queue_out, arg_manager);
    processor.do_job();

    // Write output lines
    DataWriter writer(queue_out, arg_manager);
    writer.do_job();

    return 0;
}
