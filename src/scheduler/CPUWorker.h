#pragma once

#include <thread>

class FCFSScheduler;

class CPUWorker
{
public:
    CPUWorker(int coreId, FCFSScheduler* scheduler);

    void start();
    void join();

private:
    void run();

    int m_coreId;
    FCFSScheduler* m_scheduler;
    std::thread m_thread;
};