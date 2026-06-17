#include "scheduler/FCFSScheduler.h"

#include <iostream>
#include <string>

int main()
{
    FCFSScheduler scheduler;

    scheduler.createProcesses(10);

    scheduler.start();
    std::string command;

    while (true)
    {
        std::getline(std::cin, command);

        if (command == "screen -ls")
        {
            scheduler.screen_ls();
        }
        else if (command == "exit")
        {
            break;
        }
    }

    scheduler.stop();

    return 0;
}