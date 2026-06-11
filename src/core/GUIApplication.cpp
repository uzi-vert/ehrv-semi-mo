#include "GUIApplication.h"
#include "UIConfig.h"
#include "UIManager.h"
#include "components/desktop/Desktop.h"
#include "components/taskbar/Taskbar.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>

namespace csopesy {

static void glfwErrorCallback(int error, const char* description) {
    std::cerr << "[GLFW Error " << error << "]: " << description << std::endl;
}

GUIApplication::GUIApplication() = default;

GUIApplication::~GUIApplication() {
    shutdown();
}

bool GUIApplication::initialize() {
    bootstrap();
    kernelInit();
    startSystemServices();
    return true;
}

void GUIApplication::bootstrap() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1280, 720, "CSOPESY", nullptr, nullptr);
    if (!m_window) {
        std::cerr << "[ERROR] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] Failed to initialize GLAD" << std::endl;
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void GUIApplication::kernelInit() {
    int monitorWidth, monitorHeight;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        monitorWidth = mode->width;
        monitorHeight = mode->height;
    } else {
        glfwGetWindowSize(m_window, &monitorWidth, &monitorHeight);
    }

    UIConfig::getInstance().calculateScaling(monitorWidth, monitorHeight);
    UIManager::getInstance().initialize();
}

void GUIApplication::startSystemServices() {
    m_desktop = std::make_shared<Desktop>();
    UIManager::getInstance().registerWindow("desktop", m_desktop);
    UIManager::getInstance().showWindow("desktop");

    m_taskbar = std::make_shared<Taskbar>();
    UIManager::getInstance().registerWindow("taskbar", m_taskbar);
    UIManager::getInstance().showWindow("taskbar");
}

void GUIApplication::run() {
    while (!glfwWindowShouldClose(m_window) && !UIManager::getInstance().isShutdownRequested()) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_desktop->draw();
        m_taskbar->draw();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
    }

    shutdown();
}

void GUIApplication::shutdown() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}

} // namespace csopesy
