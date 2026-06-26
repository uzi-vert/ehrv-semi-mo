#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace csopesy {
namespace scheduler {

enum class InstructionType : uint8_t {
    PRINT,
    DECLARE,
    ADD,
    SUBTRACT,
    SLEEP,
    FOR_LOOP
};

struct Operand {
    bool isLiteral = false;
    uint16_t literal = 0;
    std::string var;
};

struct Instruction {
    InstructionType type;

    std::string varName;
    uint16_t value = 0;

    Operand lhs, rhs;

    uint8_t sleepTicks = 0;

    std::string printMessage;
    std::string printVar;

    std::vector<Instruction> body;
    int repeats = 0;
};

inline int countInstructions(const std::vector<Instruction>& instrs) {
    int count = 0;
    for (const auto& ins : instrs) {
        if (ins.type == InstructionType::FOR_LOOP)
            count += ins.repeats * countInstructions(ins.body);
        else
            count++;
    }
    return count;
}

inline std::vector<Instruction> generateRandomInstructions(int count, int nestDepth = 0) {
    std::vector<Instruction> result;
    result.reserve(count);

    for (int i = 0; i < count; i++) {
        int roll = rand() % 10;
        Instruction ins;

        if (roll < 6) {
            ins.type = InstructionType::PRINT;
            ins.printMessage = "Hello world from process!";
        } else if (roll == 6) {
            ins.type = InstructionType::DECLARE;
            ins.varName = "x";
            ins.value = (uint16_t)(rand() % 100);
        } else if (roll == 7) {
            ins.type = InstructionType::ADD;
            ins.varName = "x";
            ins.lhs.isLiteral = false; ins.lhs.var = "x";
            ins.rhs.isLiteral = true;  ins.rhs.literal = 1;
        } else if (roll == 8) {
            ins.type = InstructionType::SUBTRACT;
            ins.varName = "x";
            ins.lhs.isLiteral = false; ins.lhs.var = "x";
            ins.rhs.isLiteral = true;  ins.rhs.literal = 1;
        } else {
            if (nestDepth < 3 && count > 3) {
                ins.type = InstructionType::FOR_LOOP;
                ins.repeats = 2 + rand() % 3;
                int bodyCount = 1 + rand() % 3;
                ins.body = generateRandomInstructions(bodyCount, nestDepth + 1);
            } else {
                ins.type = InstructionType::PRINT;
            }
        }
        result.push_back(std::move(ins));
    }
    return result;
}

} // namespace scheduler
} // namespace csopesy
