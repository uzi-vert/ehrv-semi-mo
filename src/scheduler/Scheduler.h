#pragma once
#include <vector>
#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include "Config.h"
#include "Process.h"

namespace csopesy {
namespace scheduler {

class Scheduler {
public:
    explicit Scheduler(const Config& cfg);
    ~Scheduler();

    void start();
    void stop();

    std::shared_ptr<Process> createProcess(const std::string& name);
    std::shared_ptr<Process> findProcess(const std::string& name);

    void startGenerating();
    void stopGenerating();

    struct Snapshot {
        int cpuCycle;
        int numCores;
        std::vector<std::shared_ptr<Process>> allProcesses;
        std::vector<int> coreAssignments;
    };
    Snapshot getSnapshot();

    std::mutex mtx;

private:
    Config cfg;
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::deque<std::shared_ptr<Process>> readyQueue;
    std::vector<std::shared_ptr<Process>> coreSlots;
    std::vector<int> coreQuantumTicks;

    std::thread tickThread;
    std::atomic<bool> running{false};
    std::atomic<bool> generating{false};

    int cpuCycle = 0;
    int nextProcessId = 1;
    int nextProcessNumber = 1;

    std::vector<int> coreDelayCounter;

    void tickLoop();
    std::shared_ptr<Process> makeProcess(const std::string& name);
    std::string nowTimestamp();
};

} // namespace scheduler
} // namespace csopesy
