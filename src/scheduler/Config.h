#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace csopesy {
namespace scheduler {

struct Config {
    int numCpu = 4;
    std::string scheduler = "rr";
    int quantumCycles = 5;
    int batchProcessFreq = 1;
    int minIns = 1000;
    int maxIns = 2000;
    int delayPerExec = 0;

    bool load(const std::string& filename = "config.txt") {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open " << filename << "\n";
            return false;
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string key;
            ss >> key;
            if (key == "num-cpu")              ss >> numCpu;
            else if (key == "scheduler") {
                ss >> scheduler;
                if (!scheduler.empty() && scheduler.front() == '"') scheduler = scheduler.substr(1);
                if (!scheduler.empty() && scheduler.back()  == '"') scheduler.pop_back();
            }
            else if (key == "quantum-cycles")      ss >> quantumCycles;
            else if (key == "batch-process-freq")  ss >> batchProcessFreq;
            else if (key == "min-ins")             ss >> minIns;
            else if (key == "max-ins")             ss >> maxIns;
            else if (key == "delay-per-exec")      ss >> delayPerExec;
        }

        if (minIns > maxIns) {
            std::cerr << "Error: min-ins must be <= max-ins\n";
            return false;
        }
        return true;
    }
};

} // namespace scheduler
} // namespace csopesy
