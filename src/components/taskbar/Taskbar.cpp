#include "Taskbar.h"
#include "core/UIManager.h"

#include <cstdlib>
#include "imgui.h"

namespace csopesy {

Taskbar::Taskbar() : AWindow("Taskbar") {
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
        ImGuiWindowFlags_NoTitleBar  |
        ImGuiWindowFlags_NoResize    |
        ImGuiWindowFlags_NoMove      |
        ImGuiWindowFlags_NoCollapse  |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 winPos  = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    drawList->AddRectFilled(winPos,
        ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
        IM_COL32(25, 25, 35, 220));
    drawList->AddLine(winPos,
        ImVec2(winPos.x + winSize.x, winPos.y),
        IM_COL32(60, 60, 80, 160));

    drawIconArea();
    drawPowerButton();

    ImGui::SetNextWindowPos(
        ImVec2(displaySize.x - 8.0f - 180.0f,
               displaySize.y - taskbarHeight - 80.0f - 8.0f),
        ImGuiCond_Always);

    if (ImGui::BeginPopup("##PowerPopup")) {
        ImGui::Text("Shut down?");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.47f, 0.12f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.16f, 0.16f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.90f, 0.20f, 0.20f, 1.0f));
        if (ImGui::Button("Yes", ImVec2(68, 26))) {
            ImGui::CloseCurrentPopup();
            std::exit(0);
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0, 8);
        if (ImGui::Button("No", ImVec2(68, 26))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
}

void Taskbar::drawIconArea() {
    UIManager& ui = UIManager::getInstance();

    float padding  = 8.0f;
    float iconSize = taskbarHeight - padding * 2.0f;
    float labelH   = 14.0f;
    float btnH     = iconSize - labelH;

    struct BtnDef {
        const char* label;
        const char* windowKey;
        const char* icon;
        ImVec4      activeColor;
    };

    static const BtnDef buttons[] = {
        { "Files",    "files",       "[F]",  { 0.18f, 0.50f, 0.30f, 0.90f } },
        { "Settings", "settings",    "[S]",  { 0.18f, 0.35f, 0.60f, 0.90f } },
        { "Tasks",    "taskmanager", "[T]",  { 0.50f, 0.25f, 0.10f, 0.90f } },
    };

    ImGui::SetCursorPos(ImVec2(padding, padding));

    for (int i = 0; i < 3; i++) {
        const BtnDef& b = buttons[i];
        bool isOpen = ui.isWindowShown(b.windowKey);

        ImGui::PushID(i);

        if (isOpen) {
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(b.activeColor.x, b.activeColor.y, b.activeColor.z, b.activeColor.w));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImVec4(b.activeColor.x + 0.10f, b.activeColor.y + 0.10f,
                       b.activeColor.z + 0.10f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(0.14f, 0.14f, 0.22f, 0.80f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImVec4(0.24f, 0.24f, 0.36f, 0.90f));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
            ImVec4(b.activeColor.x, b.activeColor.y, b.activeColor.z, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

        if (ImGui::Button(b.icon, ImVec2(iconSize, btnH))) {
            ui.toggleWindow(b.windowKey);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        float labelX = ImGui::GetCursorPosX() - iconSize +
                       (iconSize - ImGui::CalcTextSize(b.label).x) * 0.5f;
        ImGui::SetCursorPosX(labelX);
        ImGui::PushStyleColor(ImGuiCol_Text,
            isOpen ? ImVec4(0.85f, 0.92f, 1.0f, 1.0f)
                   : ImVec4(0.55f, 0.58f, 0.65f, 1.0f));
        ImGui::TextUnformatted(b.label);
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(ImVec2(padding + (iconSize + padding) * (float)(i + 1), padding));

        ImGui::PopID();
    }
}

void Taskbar::drawPowerButton() {
    float padding = 8.0f;
    float btnW    = 80.0f;
    float btnH    = taskbarHeight - 12.0f;
    ImVec2 winSize = ImGui::GetWindowSize();

    ImGui::SetCursorPos(ImVec2(winSize.x - btnW - padding, 6.0f));

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.14f, 0.14f, 0.20f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.14f, 0.14f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.30f, 0.10f, 0.10f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(0.55f, 0.00f, 0.00f, 1.00f));

    if (ImGui::Button("PWR", ImVec2(btnW, btnH))) {
        ImGui::OpenPopup("##PowerPopup");
    }

    ImGui::PopStyleColor(4);
}

} // namespace csopesy
