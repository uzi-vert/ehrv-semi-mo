#include "Taskbar.h"

#include "imgui.h"

#include <algorithm>

namespace csopesy {

Taskbar::Taskbar()
{
}

void Taskbar::draw() {
    drawTaskbarBackground();
    drawIconArea();
    drawPowerButton();
}

void Taskbar::drawTaskbarBackground() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImVec2 bgMin(0, displaySize.y - taskbarHeight);
    ImVec2 bgMax(displaySize.x, displaySize.y);

    drawList->AddRectFilled(bgMin, bgMax, IM_COL32(25, 25, 35, 200));

    drawList->AddLine(
        ImVec2(0, displaySize.y - taskbarHeight),
        ImVec2(displaySize.x, displaySize.y - taskbarHeight),
        IM_COL32(60, 60, 80, 160)
    );
}

void Taskbar::drawIconArea() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    float padding = 8.0f;
    float iconAreaY = displaySize.y - taskbarHeight + padding;
    float iconSize = taskbarHeight - padding * 2;

    for (size_t i = 0; i < icons.size(); i++) {
        float x = padding + i * (iconSize + padding);
        drawList->AddRectFilled(
            ImVec2(x, iconAreaY),
            ImVec2(x + iconSize, iconAreaY + iconSize),
            IM_COL32(50, 50, 65, 200),
            4.0f
        );
    }
}

void Taskbar::drawPowerButton() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    float padding = 8.0f;
    float btnWidth = 80.0f;
    float bgMinX = displaySize.x - padding - btnWidth;
    float bgMinY = displaySize.y - taskbarHeight + 6.0f;
    float bgMaxX = displaySize.x - padding;
    float bgMaxY = displaySize.y - 6.0f;

    drawList->AddRectFilled(
        ImVec2(bgMinX, bgMinY),
        ImVec2(bgMaxX, bgMaxY),
        IM_COL32(35, 35, 50, 160),
        4.0f
    );

    const char* text = "PWR";
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float textX = bgMinX + (btnWidth - textSize.x) / 2;
    float textY = bgMinY + ((bgMaxY - bgMinY) - textSize.y) / 2;
    drawList->AddText(ImVec2(textX, textY), IM_COL32(139, 0, 0, 255), text);
}

void Taskbar::addIcon(const TaskbarIcon& icon) {
    icons.push_back(icon);
}

void Taskbar::removeIcon(const std::string& name) {
    icons.erase(
        std::remove_if(icons.begin(), icons.end(),
            [&](const TaskbarIcon& icon) { return icon.name == name; }),
        icons.end()
    );
}

} // namespace csopesy
