#include "Taskbar.h"
#include "core/UIManager.h"

#include "imgui.h"

#include <algorithm>

namespace csopesy {

Taskbar::Taskbar()
    : AWindow("Taskbar")
{
    show();
}

void Taskbar::draw() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - taskbarHeight));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, taskbarHeight));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::Begin("##Taskbar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    drawList->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), IM_COL32(25, 25, 35, 220));
    drawList->AddLine(winPos, ImVec2(winPos.x + winSize.x, winPos.y), IM_COL32(60, 60, 80, 160));

    drawIconArea();
    drawPowerButton();

    if (showPowerPopup) {
        float popupW = 180.0f;
        float popupH = 80.0f;
        ImGui::SetNextWindowPos(
            ImVec2(displaySize.x - 8.0f - popupW, displaySize.y - taskbarHeight - popupH - 8.0f),
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(ImVec2(popupW, popupH));
        ImGui::SetNextWindowBgAlpha(0.95f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);

        ImGui::Begin("##PowerPopup", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar);

        ImGui::Text("Shut down?");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.47f, 0.12f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.16f, 0.16f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.90f, 0.20f, 0.20f, 1.0f));
        if (ImGui::Button("Yes", ImVec2(68, 26))) {
            UIManager::getInstance().requestShutdown();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0, 8);

        if (ImGui::Button("No", ImVec2(68, 26))) {
            showPowerPopup = false;
        }

        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(0)) {
            showPowerPopup = false;
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
}

void Taskbar::drawIconArea() {
    float padding  = 8.0f;
    float iconSize = taskbarHeight - padding * 2;

    ImGui::SetCursorPos(ImVec2(padding, padding));

    for (size_t i = 0; i < icons.size(); i++) {
        ImGui::PushID(static_cast<int>(i));
        ImGui::Button(icons[i].name.c_str(), ImVec2(iconSize, iconSize));
        ImGui::SameLine(0, padding);
        ImGui::PopID();
    }
}

void Taskbar::drawPowerButton() {
    float padding  = 8.0f;
    float btnW     = 80.0f;
    float btnH     = taskbarHeight - 12.0f;
    ImVec2 winSize = ImGui::GetWindowSize();

    ImGui::SetCursorPos(ImVec2(winSize.x - btnW - padding, 6.0f));

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.14f, 0.14f, 0.20f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.14f, 0.14f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.30f, 0.10f, 0.10f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(0.55f, 0.00f, 0.00f, 1.00f));

    if (ImGui::Button("PWR", ImVec2(btnW, btnH))) {
        showPowerPopup = !showPowerPopup;
    }

    ImGui::PopStyleColor(4);
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
