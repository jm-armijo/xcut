#include "ArgManager.hpp"
#include "Master.hpp"

int main(int argc, char **argv)
{
    ArgManager arg_manager;

    if (!arg_manager.processArgs(argc, argv)) {
        arg_manager.showHelp();
    } else if (arg_manager.isHelpRequested()) {
        arg_manager.showHelp();
    } else {
        auto args = arg_manager.getArgs();

        Master master(args);
        master.startWorkers();

        while(!master.workersDone());
    }

    return 0;
}

