#pragma once

#include <string>
#include <vector>

namespace csopesy {

struct TaskbarIcon {
    std::string name;
};

class Taskbar {
public:
    Taskbar();
    ~Taskbar() = default;

    void draw();

    void addIcon(const TaskbarIcon& icon);
    void removeIcon(const std::string& name);

private:
    void drawTaskbarBackground();
    void drawIconArea();
    void drawPowerButton();

    std::vector<TaskbarIcon> icons;
    float taskbarHeight = 96.0f;
};

} // namespace csopesy
