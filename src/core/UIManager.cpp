#include "UIManager.h"

namespace csopesy {

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::initialize() {
    windows.clear();
}

void UIManager::registerWindow(const std::string& name, std::shared_ptr<AWindow> window) {
    windows[name] = window;
}

void UIManager::showWindow(const std::string& name) {
    auto it = windows.find(name);
    if (it != windows.end()) {
        it->second->show();
    }
}

void UIManager::hideWindow(const std::string& name) {
    auto it = windows.find(name);
    if (it != windows.end()) {
        it->second->hide();
    }
}

void UIManager::toggleWindow(const std::string& name) {
    auto it = windows.find(name);
    if (it != windows.end()) {
        if (it->second->isShown()) it->second->hide();
        else                       it->second->show();
    }
}

bool UIManager::isWindowShown(const std::string& name) const {
    auto it = windows.find(name);
    return (it != windows.end()) && it->second->isShown();
}

void UIManager::renderAllWindows() {
    for (auto& [name, window] : windows) {
        if (window->isShown()) {
            window->draw();
        }
    }
}

} // namespace csopesy