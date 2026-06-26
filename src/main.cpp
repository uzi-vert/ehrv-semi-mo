#include "scheduler/Scheduler.h"
#include "scheduler/Config.h"
#include "core/GUIApplication.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace csopesy::scheduler;

static void clearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

static std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

enum class ConsoleState { UNINITIALIZED, MAIN_MENU, PROCESS_SCREEN };

static void printHeader()
{
    std::cout
        << "   ___  ____   __  ____  ____  ____  _  _ \n"
        << "  / __)/ ___) /  \\(  _ \\(  __)/ ___)( \\/ )\n"
        << " ( (__ \\___ \\(  O )) __/ ) _) \\___ \\ )  / \n"
        << "  \\___)(____/ \\__/(__)  (____)(____/(__/  \n"
        << "--------------------------------------\n"
        << "Welcome to CSOPESY Emulator!\n"
        << "\n"
        << "Last updated: 01-18-2024\n"
        << "--------------------------------------\n";
}

static void printHelp(ConsoleState state)
{
    if (state == ConsoleState::PROCESS_SCREEN) {
        std::cout
            << "\n  Available commands:\n"
            << "    process-smi   Display process details\n"
            << "    exit          Return to main menu\n"
            << "    help          Show this help message\n"
            << "\n";
    } else {
        std::cout
            << "\n  Available commands:\n"
            << "    initialize             Read config.txt and initialize the system\n"
            << "    screen -s <name>       Create a new process and open its screen\n"
            << "    screen -r <name>       Access an existing process screen\n"
            << "    screen -ls             List all processes and CPU utilization\n"
            << "    scheduler-start        Start the scheduler and batch generation\n"
            << "    scheduler-stop         Stop the scheduler and batch generation\n"
            << "    report-util            Write current status to csopesy-log.txt\n"
            << "    help                   Show this help message\n"
            << "    exit                   Shut down the system\n"
            << "\n";
    }
}

static void showProcessSmi(Scheduler& scheduler, std::shared_ptr<Process> proc)
{
    if (!proc) return;

    std::shared_ptr<Process> p = proc;
    std::string name, createdAt;
    int id, currentLine, total;
    bool done;
    std::vector<std::string> logs;

    {
        std::lock_guard<std::mutex> lock(scheduler.mtx);
        name        = p->name;
        id          = p->id;
        createdAt   = p->createdAt;
        currentLine = p->getCurrentLine();
        total       = p->totalInstructions;
        done        = p->hasFinished();
        logs        = p->logs;
    }

    std::cout << "Process name: " << name << "\n";
    std::cout << "ID: " << id << "\n";
    std::cout << "Logs:\n";
    int start = std::max(0, (int)logs.size() - 10);
    for (int i = start; i < (int)logs.size(); i++)
        std::cout << logs[i] << "\n";
    if (logs.size() > 10)
        std::cout << "  ... (" << logs.size() - 10 << " earlier entries)\n";
    std::cout << "\n";
    if (done) {
        std::cout << "Finished!\n";
    } else {
        std::cout << "Current instruction line: " << currentLine << "\n";
        std::cout << "Lines of code: " << total << "\n";
    }
}

static void printProcessList(Scheduler& scheduler, std::ostream& out)
{
    auto snap = scheduler.getSnapshot();

    int busyCores = 0;
    for (int id : snap.coreAssignments) if (id != -1) busyCores++;
    int pct = (snap.numCores > 0) ? (busyCores * 100 / snap.numCores) : 0;

    out << "CPU utilization: " << pct << "%\n";
    out << "Cores used: " << busyCores << "\n";
    out << "Cores available: " << (snap.numCores - busyCores) << "\n";
    out << "--------------------------------------\n";

    std::vector<std::pair<std::shared_ptr<Process>, int>> onCore;
    std::vector<std::shared_ptr<Process>> finished;
    for (auto& p : snap.allProcesses) {
        if (p->hasFinished()) {
            finished.push_back(p);
            continue;
        }
        int core = -1;
        for (int i = 0; i < snap.numCores; i++)
            if (snap.coreAssignments[i] == p->id) { core = i; break; }
        if (core != -1)
            onCore.push_back({p, core});
    }

    out << "Running processes:\n";
    for (auto& [p, core] : onCore) {
        out << std::left << std::setw(12) << p->name
            << " " << p->createdAt
            << "    Core: " << core
            << "    " << p->getCurrentLine() << " / " << p->totalInstructions
            << "\n";
    }

    out << "\nFinished processes:\n";
    for (auto& p : finished) {
        out << std::left << std::setw(12) << p->name
            << " " << p->createdAt
            << "    Finished"
            << "    " << p->totalInstructions << " / " << p->totalInstructions
            << "\n";
    }
    out << "--------------------------------------\n";
}

static int consoleMain()
{
    ConsoleState state = ConsoleState::UNINITIALIZED;
    std::unique_ptr<Scheduler> scheduler;
    std::shared_ptr<Process> activeProcess;

    printHeader();

    std::string line;
    while (true) {
        if (state == ConsoleState::PROCESS_SCREEN && activeProcess)
            std::cout << "root:\\> ";
        else
            std::cout << "root:\\> ";
        std::cout.flush();

        if (!std::getline(std::cin, line)) break;

        while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
            line.pop_back();

        if (line.empty()) continue;

        if (state == ConsoleState::PROCESS_SCREEN) {
            if (line == "exit") {
                state = ConsoleState::MAIN_MENU;
                activeProcess = nullptr;
                printHeader();
                continue;
            }
            if (line == "process-smi") {
                showProcessSmi(*scheduler, activeProcess);
                continue;
            }
            if (line == "help") {
                printHelp(ConsoleState::PROCESS_SCREEN);
                continue;
            }
            std::cout << "Unknown command: " << line << "\n";
            continue;
        }

        // MAIN_MENU / UNINITIALIZED
        if (line == "exit") {
            if (scheduler) scheduler->stop();
            std::cout << "Goodbye!\n";
            return 0;
        }

        if (line == "initialize") {
            Config cfg;
            if (!cfg.load()) {
                std::cout << "Failed to load config.txt\n";
                continue;
            }
            if (scheduler) {
                scheduler->stopGenerating();
                scheduler->stop();
            }
            scheduler = std::make_unique<Scheduler>(cfg);
            scheduler->start();
            state = ConsoleState::MAIN_MENU;
            std::cout << "Initialized.\n";
            continue;
        }

        if (state == ConsoleState::UNINITIALIZED) {
            std::cout << "Please run 'initialize' first.\n";
            continue;
        }

        if (line.rfind("screen", 0) == 0) {
            std::string args = line.substr(6);
            std::istringstream ss(args);
            std::string flag;
            ss >> flag;

            if (flag == "-s") {
                std::string name;
                ss >> name;
                if (name.empty()) { std::cout << "Usage: screen -s <name>\n"; continue; }
                auto proc = scheduler->createProcess(name);
                activeProcess = proc;
                state = ConsoleState::PROCESS_SCREEN;
                std::cout << "\n  Process " << name << " created successfully.\n";
                std::cout << "  Accessing process " << name << "...\n\n";
                showProcessSmi(*scheduler, activeProcess);
            } else if (flag == "-r") {
                std::string name;
                ss >> name;
                if (name.empty()) { std::cout << "Usage: screen -r <name>\n"; continue; }
                auto proc = scheduler->findProcess(name);
                if (!proc) {
                    std::cout << "  Process " << name << " not found.\n";
                    continue;
                }
                activeProcess = proc;
                state = ConsoleState::PROCESS_SCREEN;
                std::cout << "\n  Accessing process " << name << "...\n\n";
                showProcessSmi(*scheduler, activeProcess);
            } else if (flag == "-ls") {
                printProcessList(*scheduler, std::cout);
            } else {
                std::cout << "Usage: screen -s <name> | screen -r <name> | screen -ls\n";
            }
            continue;
        }

        if (line == "scheduler-start") {
            if (!scheduler) { std::cout << "Not initialized.\n"; continue; }
            scheduler->startGenerating();
            std::cout << "Scheduler started.\n";
            continue;
        }
        if (line == "scheduler-stop") {
            if (!scheduler) { std::cout << "Not initialized.\n"; continue; }
            scheduler->stopGenerating();
            std::cout << "Scheduler stopped.\n";
            continue;
        }
        if (line == "report-util") {
            if (!scheduler) { std::cout << "Not initialized.\n"; continue; }
            std::ofstream file("csopesy-log.txt");
            if (!file.is_open()) { std::cout << "Failed to write csopesy-log.txt\n"; continue; }
            printProcessList(*scheduler, file);
            file.close();
            std::cout << "Report generated at csopesy-log.txt\n";
            continue;
        }
        if (line == "help") {
            printHelp(ConsoleState::MAIN_MENU);
            continue;
        }

        std::cout << "Unknown command: " << line << "\n";
    }

    if (scheduler) scheduler->stop();
    return 0;
}

int main(int argc, char* argv[])
{
    bool consoleMode = false;

    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "--console")
        {
            consoleMode = true;
            break;
        }
    }

    if (consoleMode)
    {
        return consoleMain();
    }

    csopesy::GUIApplication app;

    if (!app.initialize()) {
        return 1;
    }

    app.run();

    return 0;
}
