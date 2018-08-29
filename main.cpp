#include "ArgManager.hpp"
#include "DataProcessor.hpp"
#include "DataQueue.hpp"
#include "DataReader.hpp"
#include "DataWriter.hpp"

enum class Status {reading, processing, writing, done};

void do_job(ArgManager& arg_manager);

int main(int argc, char **argv)
{
    ArgManager arg_manager;
    arg_manager.readArgs(argc, argv);

    if (arg_manager.getHelp()) {
        arg_manager.printHelp();
        return 1;
    }

    do_job(arg_manager);

    return 0;
}

void do_job(ArgManager& arg_manager)
{
    DataQueue queue_in;
    DataQueue queue_out;

    // Read input lines
    DataReader reader(queue_in);
    reader.do_job();

    // Process lines
    DataProcessor processor(queue_in, queue_out, arg_manager);
    processor.do_job();

    // Write output lines
    DataWriter writer(queue_out, arg_manager);
    writer.do_job();

    auto loop = true;
    auto status = Status::reading;
    while (loop) {
        switch (status) {
            case Status::reading:
                if (reader.done()) {
                    status = Status::processing;
                }
                break;
            case Status::processing:
                if (processor.done()) {
                    status = Status::writing;
                }
                break;
            case Status::writing:
                if (writer.done()) {
                    status = Status::done;
                }
                break;
            default:
                loop = false;
                break;
        }
    }

    return;
}
