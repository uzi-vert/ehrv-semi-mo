#include "CPUWorker.h"
#include "FCFSScheduler.h"
#include "Process.h"

#include <fstream>
#include <chrono>
#include <thread>

CPUWorker::CPUWorker(int coreId, FCFSScheduler* scheduler)
    : m_coreId(coreId),
      m_scheduler(scheduler)
{
}

void CPUWorker::start()
{
    m_thread = std::thread(&CPUWorker::run, this);
}

void CPUWorker::join()
{
    if (m_thread.joinable())
        m_thread.join();
}

void CPUWorker::run()
{
    while (m_scheduler->isRunning())
    {
        auto process = m_scheduler->getNextProcess();

        if (!process)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(10));
            continue;
        }

        process->state = ProcessState::RUNNING;
        process->assignedCore = m_coreId;

        std::ofstream file(process->name + ".txt");

        for (int i = 0; i < process->totalPrints; i++)
        {
            file
                << m_scheduler->getTimestamp()
                << " Core:"
                << m_coreId
                << " \"Hello world from "
                << process->name
                << "\"\n";

            process->completedPrints++;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(50));
        }

        file.close();

        process->finished = true;
        process->state = ProcessState::FINISHED;

        m_scheduler->markFinished(process);
    }
}