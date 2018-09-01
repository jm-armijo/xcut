#include "ArgManager.hpp"
#include "Master.hpp"

int main(int argc, char **argv)
{
    ArgManager arg_manager;

    if (!arg_manager.processArgs(argc, argv)) {
        // Something went wrong. Show help and exit.
        arg_manager.printHelp();
    } else {
        auto args = arg_manager.getArgs();

        // Help requested. Show help and exit.
        if (args.get("-h") == "1") {
            arg_manager.printHelp();
        } else {
            Master master(args);
            master.start_workers();

            while(!master.workers_done());
       }
    }

    return 0;
}

