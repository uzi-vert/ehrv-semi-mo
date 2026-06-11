#include "Desktop.h"
#include "core/UIConfig.h"

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <chrono>
#include <ctime>
#include <iostream>

namespace csopesy {

Desktop::Desktop()
    : AWindow("Desktop")
{
    loadWallpaper();
}

Desktop::~Desktop() {
    if (wallpaperTexture) {
        glDeleteTextures(1, &wallpaperTexture);
    }
}

void Desktop::loadWallpaper() {
    int channels;
    unsigned char* data = stbi_load("assets/wallpaper.png", &wallpaperWidth, &wallpaperHeight, &channels, 4);

    if (!data) {
        std::cerr << "[WARNING] Failed to load wallpaper from assets/wallpaper.png — using fallback background" << std::endl;
        wallpaperLoaded = false;
        return;
    }

    glGenTextures(1, &wallpaperTexture);
    glBindTexture(GL_TEXTURE_2D, wallpaperTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wallpaperWidth, wallpaperHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    wallpaperLoaded = true;
}

void Desktop::draw() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(displaySize);

    ImGui::Begin(windowName.c_str(), nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoMouseInputs);

    drawWallpaper();
    drawClock();

    ImGui::End();
}

void Desktop::drawWallpaper() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    if (wallpaperLoaded && wallpaperTexture) {
        drawList->AddImage(
            (void*)(intptr_t)wallpaperTexture,
            ImVec2(0, 0),
            displaySize
        );
    } else {
        drawList->AddRectFilled(
            ImVec2(0, 0),
            displaySize,
            IM_COL32(20, 20, 30, 255)
        );
    }
}

void Desktop::drawClock() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%I:%M:%S %p", &timeinfo);

    ImVec2 textSize = ImGui::CalcTextSize(buffer);
    float padding = 20.0f;
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImVec2 bgMin(
        displaySize.x - textSize.x - padding * 2,
        padding
    );
    ImVec2 bgMax(
        displaySize.x - padding,
        textSize.y + padding * 2
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(bgMin, bgMax, IM_COL32(0, 0, 0, 128), 8.0f);

    ImVec2 textPos(
        displaySize.x - textSize.x - padding * 1.5f,
        padding * 1.5f
    );
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), buffer);
}

} // namespace csopesy