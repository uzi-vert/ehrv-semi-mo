#pragma once

#include "AWindow.h"
#include <map>
#include <memory>
#include <string>

namespace csopesy {

class UIManager {
public:
    static UIManager& getInstance();

    void initialize();

    void registerWindow(const std::string& name, std::shared_ptr<AWindow> window);
    void showWindow(const std::string& name);
    void hideWindow(const std::string& name);
    void toggleWindow(const std::string& name);
    bool isWindowShown(const std::string& name) const;

    void renderAllWindows();

private:
    UIManager() = default;

    std::map<std::string, std::shared_ptr<AWindow>> windows;
};

} // namespace csopesy