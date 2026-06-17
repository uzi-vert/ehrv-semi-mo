#pragma once

#include <string>
#include <atomic>

enum class ProcessState
{
    READY,
    RUNNING,
    FINISHED
};

struct Process
{
    std::string name;
    std::string timestamp;

    int totalPrints;

    std::atomic<int> completedPrints;
    std::atomic<int> assignedCore;
    std::atomic<bool> finished;

    ProcessState state;

    Process(const std::string& n, int prints)
        : name(n),
          totalPrints(prints),
          completedPrints(0),
          assignedCore(-1),
          finished(false),
          state(ProcessState::READY)
    {
    }
};