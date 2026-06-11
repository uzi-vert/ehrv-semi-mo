#pragma once

#include "core/AWindow.h"
#include <string>
#include <vector>

namespace csopesy {

struct TaskbarIcon {
    std::string name;
};

class Taskbar : public AWindow {
public:
    Taskbar();
    ~Taskbar() = default;

    void draw() override;

    void addIcon(const TaskbarIcon& icon);
    void removeIcon(const std::string& name);

private:
    void drawIconArea();
    void drawPowerButton();

    std::vector<TaskbarIcon> icons;
    float taskbarHeight = 96.0f;
    bool showPowerPopup = false;
};

} // namespace csopesy
