#include "Process.h"
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace csopesy {
namespace scheduler {

static std::string nowTimestamp() {
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

int Process::countInstructions(const std::vector<Instruction>& instrs) {
    int count = 0;
    for (const auto& ins : instrs) {
        if (ins.type == InstructionType::FOR_LOOP)
            count += ins.repeats * countInstructions(ins.body);
        else
            count++;
    }
    return count;
}

Process::Process(const std::string& name, int id, const std::string& ts,
                 std::vector<Instruction> instrs)
    : name(name), id(id), createdAt(ts), instructions(std::move(instrs))
{
    totalInstructions = countInstructions(instructions);
    callStack.push_back({&instructions, 0, 1});
}

bool Process::hasFinished() const { return finished; }
int  Process::getCurrentLine() const { return executedInstructions; }

uint16_t Process::resolveVar(const std::string& var) {
    auto it = variables.find(var);
    return (it != variables.end()) ? it->second : 0;
}

uint16_t Process::clampUint16(int val) {
    if (val < 0) return 0;
    if (val > 65535) return 65535;
    return (uint16_t)val;
}

void Process::executeInstruction(const Instruction& instr, int core) {
    switch (instr.type) {
    case InstructionType::PRINT: {
        std::string ts = nowTimestamp();
        std::ostringstream msg;
        msg << ts << " Core:" << core << " \"";
        if (!instr.printMessage.empty()) {
            msg << instr.printMessage;
            if (!instr.printVar.empty()) {
                msg << resolveVar(instr.printVar);
            }
        } else {
            msg << "Hello world from " << name << "!";
        }
        msg << "\"";
        logs.push_back(msg.str());
        break;
    }
    case InstructionType::DECLARE:
        variables[instr.varName] = instr.value;
        break;
    case InstructionType::ADD: {
        uint16_t lhs = instr.lhs.isLiteral ? instr.lhs.literal : resolveVar(instr.lhs.var);
        uint16_t rhs = instr.rhs.isLiteral ? instr.rhs.literal : resolveVar(instr.rhs.var);
        variables[instr.varName] = clampUint16((int)lhs + (int)rhs);
        break;
    }
    case InstructionType::SUBTRACT: {
        uint16_t lhs = instr.lhs.isLiteral ? instr.lhs.literal : resolveVar(instr.lhs.var);
        uint16_t rhs = instr.rhs.isLiteral ? instr.rhs.literal : resolveVar(instr.rhs.var);
        variables[instr.varName] = clampUint16((int)lhs - (int)rhs);
        break;
    }
    case InstructionType::SLEEP:
        sleepTicksLeft = (int)instr.sleepTicks;
        break;
    case InstructionType::FOR_LOOP:
        callStack.push_back({&instr.body, 0, instr.repeats});
        break;
    }
}

void Process::executeNextInstruction(int core) {
    if (finished) return;

    if (sleepTicksLeft > 0) {
        sleepTicksLeft--;
        return;
    }

    while (!callStack.empty()) {
        Frame& frame = callStack.back();

        if (frame.index < (int)frame.body->size()) {
            const Instruction& instr = (*frame.body)[frame.index];
            frame.index++;

            if (instr.type == InstructionType::FOR_LOOP) {
                callStack.push_back({&instr.body, 0, instr.repeats});
                return;
            }

            executeInstruction(instr, core);
            executedInstructions++;
            return;
        } else {
            frame.remainingRepeats--;
            if (frame.remainingRepeats > 0)
                frame.index = 0;
            else
                callStack.pop_back();
        }
    }

    finished = true;
    state = ProcessState::FINISHED;
}

} // namespace scheduler
} // namespace csopesy
