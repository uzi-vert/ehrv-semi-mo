#pragma once

#include "Process.h"

#include <deque>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <unordered_map>

namespace csopesy {
namespace scheduler {

class CPUWorker;

class FCFSScheduler
{
public:
    FCFSScheduler(bool generateFiles = true);
    ~FCFSScheduler();

    void initializeConfig(const std::unordered_map<std::string, std::string>& config);
    int getDelayPerExec() const { return delayPerExec; }

    void start();
    void stop();

    bool isRunning();
    bool shouldGenerateFiles() const;

    void createProcesses(int count);

    std::shared_ptr<Process> getNextProcess();

    void markFinished(std::shared_ptr<Process> process);

    void screen_ls();
    void exportReport();

    std::string getTimestamp();

private:
    void schedulerLoop();

    std::deque<std::shared_ptr<Process>> readyQueue;

    std::vector<std::shared_ptr<Process>> runningProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;

    std::vector<std::unique_ptr<CPUWorker>> workers;

    std::mutex mtx;

    std::thread schedulerThread;

    bool running;
    bool m_generateFiles;

    // config.txt variables
    int numCpu = 4;
    int batchProcessFreq = 1;
    int minIns = 1000;
    int maxIns = 2000;
    int delayPerExec = 0;
};

} // namespace scheduler
} // namespace csopesy
