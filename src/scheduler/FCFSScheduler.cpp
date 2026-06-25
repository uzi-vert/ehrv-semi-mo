#include "FCFSScheduler.h"
#include "CPUWorker.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>

namespace csopesy {
namespace scheduler {

FCFSScheduler::FCFSScheduler(bool generateFiles)
    : running(false),
      m_generateFiles(generateFiles)
{
}

FCFSScheduler::~FCFSScheduler()
{
    stop();
}

void FCFSScheduler::initializeConfig(const std::unordered_map<std::string, std::string>& config)
{
    if (config.count("num-cpu")) 
        numCpu = std::stoi(config.at("num-cpu"));

    if (config.count("delay-per-exec")) {
        float delaySec = std::stof(config.at("delay-per-exec"));
        delayPerExec = static_cast<int>(delaySec * 1000);
    }

    if (config.count("min-ins")) 
        minIns = std::stoi(config.at("min-ins"));

    if (config.count("max-ins")) 
        maxIns = std::stoi(config.at("max-ins"));

    if (config.count("batch-process-freq")) 
        batchProcessFreq = std::stoi(config.at("batch-process-freq"));
}

void FCFSScheduler::start()
{
    running = true;

    schedulerThread =
        std::thread(
            &FCFSScheduler::schedulerLoop,
            this);

    for (int i = 0; i < numCpu; i++)
    {
        workers.push_back(
            std::make_unique<CPUWorker>(i, this));

        workers.back()->start();
    }
}

void FCFSScheduler::stop()
{
    running = false;

    if (schedulerThread.joinable())
        schedulerThread.join();

    for (auto& worker : workers)
        worker->join();
}

bool FCFSScheduler::isRunning()
{
    return running;
}

bool FCFSScheduler::shouldGenerateFiles() const
{
    return m_generateFiles;
}

void FCFSScheduler::schedulerLoop()
{
    while (running)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }
}

void FCFSScheduler::createProcesses(int count)
{
    std::lock_guard<std::mutex> lock(mtx);

    for (int i = 1; i <= count; i++)
    {
        std::stringstream ss;

        ss << "process_"
           << std::setw(2)
           << std::setfill('0')
           << i;

        auto process =
            std::make_shared<Process>(
                ss.str(),
                100);

        process->timestamp = getTimestamp();

        readyQueue.push_back(process);
    }
}

std::shared_ptr<Process>
FCFSScheduler::getNextProcess()
{
    std::lock_guard<std::mutex> lock(mtx);

    if (readyQueue.empty())
        return nullptr;

    auto process = readyQueue.front();
    readyQueue.pop_front();

    runningProcesses.push_back(process);

    return process;
}

void FCFSScheduler::markFinished(
    std::shared_ptr<Process> process)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto it =
        std::find(
            runningProcesses.begin(),
            runningProcesses.end(),
            process);

    if (it != runningProcesses.end())
        runningProcesses.erase(it);

    finishedProcesses.push_back(process);
}

void FCFSScheduler::screen_ls()
{
    std::lock_guard<std::mutex> lock(mtx);

    std::cout
        << "\n---------------------------------\n";

    std::cout
        << "Processes in the Ready Queue: "
        << readyQueue.size()
        << "\n";

    for (auto& p : readyQueue)
    {
        std::cout
            << "\t"
            << p->name
            << "\t"
            << p->timestamp
            << "\t\tReady"
            << "\n";
    }

    std::cout
        << "\nRunning Processes:\n";

    for (auto& p : runningProcesses)
    {
        std::cout
            << "\t"
            << p->name
            << "\t"
            << p->timestamp
            << "\t\tCore: "
            << p->assignedCore
            << "\t"
            << p->completedPrints
            << " / "
            << p->totalPrints
            << "\n";
    }

    std::cout
        << "\nFinished Processes:\n";

    for (auto& p : finishedProcesses)
    {
        std::cout
            << "\t"
            << p->name
            << "\t"
            << p->timestamp
            << "\t\tFinished\t"
            << p->completedPrints
            << " / "
            << p->totalPrints
            << "\n";
    }


    std::cout
        << "---------------------------------\n";
}

void FCFSScheduler::exportReport()
{
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::stringstream fss;
    fss << "scheduler_report_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S")
        << ".txt";

    std::ofstream file(fss.str());

    file
        << "Scheduler Report - "
        << getTimestamp()
        << "\n"
        << "=================================\n\n";

    file
        << "Ready Queue (" << readyQueue.size() << "):\n";

    for (auto& p : readyQueue)
    {
        file
            << "\t" << p->name
            << " | " << p->timestamp
            << " | Ready"
            << "\n";
    }

    file
        << "\nRunning Processes:\n";

    for (auto& p : runningProcesses)
    {
        file
            << "\t" << p->name
            << " | " << p->timestamp
            << " | Core: " << p->assignedCore
            << " | " << p->completedPrints
            << " / " << p->totalPrints
            << "\n";
    }

    file
        << "\nFinished Processes:\n";

    for (auto& p : finishedProcesses)
    {
        file
            << "\t" << p->name
            << " | " << p->timestamp
            << " | Finished | "
            << p->completedPrints
            << " / " << p->totalPrints
            << "\n";
    }

    file
        << "\n=================================\n";

    file.close();

    std::cout
        << "\nReport written to: "
        << fss.str()
        << "\n";
}

std::string FCFSScheduler::getTimestamp()
{
    auto now =
        std::chrono::system_clock::now();

    auto t =
        std::chrono::system_clock::to_time_t(now);

    std::tm tm;

#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::stringstream ss;

    ss << "("
       << std::put_time(
              &tm,
              "%m/%d/%Y %I:%M:%S%p")
       << ")";

    return ss.str();
}

} // namespace scheduler
} // namespace csopesy
