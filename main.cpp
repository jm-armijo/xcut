#include "ArgManager.hpp"
#include "Master.hpp"

int main(int argc, char **argv)
{
    ArgManager arg_manager;
    arg_manager.readArgs(argc, argv);

    if (arg_manager.getHelp()) {
        arg_manager.printHelp();
        return 1;
    }

    Master master(arg_manager);
    master.start_workers();

    while(!master.workers_done());

    return 0;
}

