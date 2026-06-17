#pragma once

#include "Process.h"

#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <string>

class CPUWorker;

class FCFSScheduler
{
public:
    FCFSScheduler();
    ~FCFSScheduler();

    void start();
    void stop();

    bool isRunning();

    void createProcesses(int count);

    std::shared_ptr<Process> getNextProcess();

    void markFinished(std::shared_ptr<Process> process);

    void screen_ls();

    std::string getTimestamp();

private:
    void schedulerLoop();

    std::queue<std::shared_ptr<Process>> readyQueue;

    std::vector<std::shared_ptr<Process>> runningProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;

    std::vector<std::unique_ptr<CPUWorker>> workers;

    std::mutex mtx;

    std::thread schedulerThread;

    bool running;
};