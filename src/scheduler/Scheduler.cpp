#include "Scheduler.h"
#include "Instruction.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace csopesy {
namespace scheduler {

static std::string makeTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return ss.str();
}

Scheduler::Scheduler(const Config& cfg)
    : cfg(cfg),
      coreSlots(cfg.numCpu, nullptr),
      coreQuantumTicks(cfg.numCpu, 0),
      coreDelayCounter(cfg.numCpu, 0)
{}

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::start() {
    running = true;
    tickThread = std::thread(&Scheduler::tickLoop, this);
}

void Scheduler::stop() {
    running = false;
    if (tickThread.joinable()) tickThread.join();
}

void Scheduler::startGenerating() { generating = true; }
void Scheduler::stopGenerating()  { generating = false; }

std::shared_ptr<Process> Scheduler::createProcess(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    auto proc = makeProcess(name);
    allProcesses.push_back(proc);
    readyQueue.push_back(proc);
    return proc;
}

std::shared_ptr<Process> Scheduler::findProcess(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& p : allProcesses)
        if (p->name == name) return p;
    return nullptr;
}

Scheduler::Snapshot Scheduler::getSnapshot() {
    std::lock_guard<std::mutex> lock(mtx);
    Snapshot s;
    s.cpuCycle = cpuCycle;
    s.numCores = cfg.numCpu;
    s.allProcesses = allProcesses;
    s.coreAssignments.resize(cfg.numCpu, -1);
    for (int i = 0; i < cfg.numCpu; i++)
        if (coreSlots[i]) s.coreAssignments[i] = coreSlots[i]->id;
    return s;
}

void Scheduler::tickLoop() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::lock_guard<std::mutex> lock(mtx);
        cpuCycle++;

        if (generating && (cpuCycle % cfg.batchProcessFreq == 0)) {
            std::ostringstream name;
            name << "p" << std::setw(2) << std::setfill('0') << nextProcessNumber++;
            auto proc = makeProcess(name.str());
            allProcesses.push_back(proc);
            readyQueue.push_back(proc);
        }

        for (int i = 0; i < cfg.numCpu; i++) {
            auto& proc = coreSlots[i];
            if (!proc) continue;

            bool shouldExecute = false;
            if (cfg.delayPerExec == 0) {
                shouldExecute = true;
            } else {
                coreDelayCounter[i]++;
                if (coreDelayCounter[i] > cfg.delayPerExec) {
                    coreDelayCounter[i] = 0;
                    shouldExecute = true;
                }
            }

            if (shouldExecute) {
                proc->executeNextInstruction(i);
                coreQuantumTicks[i]++;
            }

            if (proc->hasFinished()) {
                proc->coreId = -1;
                proc = nullptr;
                continue;
            }

            if (cfg.scheduler == "rr" && coreQuantumTicks[i] >= cfg.quantumCycles) {
                proc->coreId = -1;
                readyQueue.push_back(proc);
                proc = nullptr;
                coreQuantumTicks[i] = 0;
            }
        }

        for (int i = 0; i < cfg.numCpu; i++) {
            if (!coreSlots[i] && !readyQueue.empty()) {
                coreSlots[i] = readyQueue.front();
                readyQueue.pop_front();
                coreSlots[i]->coreId = i;
                coreSlots[i]->state = ProcessState::RUNNING;
                coreQuantumTicks[i] = 0;
                coreDelayCounter[i] = 0;
            }
        }
    }
}

std::shared_ptr<Process> Scheduler::makeProcess(const std::string& name) {
    int count = cfg.minIns + rand() % (cfg.maxIns - cfg.minIns + 1);
    auto instrs = generateRandomInstructions(count, 0);
    auto proc = std::make_shared<Process>(name, nextProcessId++, makeTimestamp(), std::move(instrs));
    return proc;
}

std::string Scheduler::nowTimestamp() {
    return makeTimestamp();
}

} // namespace scheduler
} // namespace csopesy
