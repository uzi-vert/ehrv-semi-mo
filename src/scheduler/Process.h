#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "Instruction.h"

namespace csopesy {
namespace scheduler {

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    FINISHED
};

class Process {
public:
    std::string name;
    int id;
    std::string createdAt;
    int totalInstructions;
    int coreId = -1;
    bool finished = false;

    std::vector<std::string> logs;
    int executedInstructions = 0;
    ProcessState state = ProcessState::NEW;

    Process(const std::string& name, int id, const std::string& timestamp,
            std::vector<Instruction> instructions);

    void executeNextInstruction(int core);
    bool hasFinished() const;
    int getCurrentLine() const;

    static int countInstructions(const std::vector<Instruction>& instrs);

private:
    struct Frame {
        const std::vector<Instruction>* body;
        int index;
        int remainingRepeats;
    };

    std::vector<Instruction> instructions;
    std::vector<Frame> callStack;
    std::unordered_map<std::string, uint16_t> variables;
    int sleepTicksLeft = 0;

    void executeInstruction(const Instruction& instr, int core);
    uint16_t resolveVar(const std::string& var);
    uint16_t clampUint16(int val);
};

} // namespace scheduler
} // namespace csopesy
