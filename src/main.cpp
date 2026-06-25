#include "scheduler/FCFSScheduler.h"
#include "core/GUIApplication.h"

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

static void clearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

static void showMarquee()
{
    clearScreen();

    std::cout
            << "  =====================================================\n"
            << "            CSOPESY Operating System Simulator\n"
            << "  =====================================================\n\n"
            << "  Welcome to CSOPESY Emulator!\n\n"
            << "  Developers:\n"
            << "  Dela Cruz, Ethan Iago\n"
            << "  Dizon, Rohann Gabriel\n"
            << "  Tandingan, Hyun Jei\n"
            << "  Valdez, Pulvert\n\n"
            << "  Type 'initialize' to run\n"
            << "  -----------------------------------------------------\n";
}

static void bootSequence()
{
    std::cout << "  Booting kernel";
    for (int i = 0; i < 3; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " OK\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "  Initializing memory manager";
    for (int i = 0; i < 3; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " OK\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "  Starting scheduler";
    for (int i = 0; i < 3; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " OK\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "  Creating 10 processes";
    for (int i = 0; i < 3; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " OK\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "  Initializing 4 CPU cores";
    for (int i = 0; i < 3; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " OK\n\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    std::cout
        << "  -----------------------------------------------------\n"
        << "  Type 'help' for available commands.\n"
        << "  -----------------------------------------------------\n\n";
}

static std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static void printHelp()
{
    std::cout
        << "\n"
        << "  Available commands:\n"
        << "    screen -ls    List running and finished processes\n"
        << "    export        Export scheduler report to file\n"
        << "    help          Show this help message\n"
        << "    exit          Shut down the system\n"
        << "\n";
}

// read key value pairs inside config.txt
static std::unordered_map<std::string, std::string> loadConfig(const std::string& filename){
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    
    if (!file.is_open()){
        std::cout << "  Error: Could not open " << filename << "\n";
        return config;
    }

    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string key, value;
        
        if (iss >> key >> value){
            config[key] = value;
        }
    }
    return config;
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
        bool isInitialized = false;
        csopesy::scheduler::FCFSScheduler scheduler; 

        showMarquee();

        std::string command;

        while (true)
        {
            std::cout << "\n  csopesy> ";
            std::cout.flush();

            std::getline(std::cin, command);
            command = trim(command);

            if (command.empty())
            {
                continue;
            }

            std::cout << "\n  > " << command << "\n";

            if (command == "exit")
            {
                std::cout << "  Shutting down...\n\n";
                break;
            }
            else if (command == "initialize")
            {
                if (isInitialized) {
                    std::cout << "  System has already been initialized.\n";
                } else {
                    bootSequence();
                    std::cout << "  Reading config.txt...\n"; 

                    auto config = loadConfig("config.txt");
                    if (config.empty()) {
                        std::cout << "  Config missing or empty.\n";
                        continue;
                    }

                    scheduler.initializeConfig(config);
                    
                    scheduler.start();
                    isInitialized = true;
                    std::cout << "  System initialized successfully.\n";
                }
            }
            else if (!isInitialized) 
            {
                std::cout << "  System not initialized. Please run 'initialize' first.\n";
            }
            else if (command == "screen -ls")
            {
                scheduler.screen_ls();
            }
            else if (command == "export")
            {
                scheduler.exportReport();
            }
            else if (command == "help")
            {
                printHelp();
            }
            else
            {
                std::cout << "  Unknown command: '" << command << "'. Type 'help'.\n";
            }
        }

        if (isInitialized) {
            scheduler.stop();
        }

        return 0;
    }

    csopesy::GUIApplication app;

    if (!app.initialize()) {
        return 1;
    }

    app.run();

    return 0;
}
