#include "FCFSScheduler.h"
#include "CPUWorker.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

FCFSScheduler::FCFSScheduler()
{
    running = false;
}

FCFSScheduler::~FCFSScheduler()
{
    stop();
}

void FCFSScheduler::start()
{
    running = true;

    schedulerThread =
        std::thread(
            &FCFSScheduler::schedulerLoop,
            this);

    for (int i = 0; i < 4; i++)
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

        readyQueue.push(process);
    }
}

std::shared_ptr<Process>
FCFSScheduler::getNextProcess()
{
    std::lock_guard<std::mutex> lock(mtx);

    if (readyQueue.empty())
        return nullptr;

    auto process = readyQueue.front();
    readyQueue.pop();

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
        << "Running Processes\n\n";

    for (auto& p : runningProcesses)
    {
        std::cout
            << p->name
            << "\t"
            << p->timestamp
            << "\tCore:"
            << p->assignedCore
            << "\t"
            << p->completedPrints
            << " / "
            << p->totalPrints
            << "\n";
    }

    std::cout
        << "\nFinished Processes\n\n";

    for (auto& p : finishedProcesses)
    {
        std::cout
            << p->name
            << "\t"
            << p->timestamp
            << "\tFinished\t"
            << p->completedPrints
            << " / "
            << p->totalPrints
            << "\n";
    }


    std::cout
        << "---------------------------------\n";
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