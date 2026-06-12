#pragma once

#include "core/AWindow.h"

namespace csopesy {

class Taskbar : public AWindow {
public:
    Taskbar();
    ~Taskbar() = default;

    void draw() override;

private:
    void drawIconArea();
    void drawPowerButton();

    float taskbarHeight = 96.0f;
};

} // namespace csopesy
