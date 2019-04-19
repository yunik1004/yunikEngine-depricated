#include <iostream>
#include <yunikEngine/projectManager.hpp>
#include <yunikEngine/window.hpp>

int main (void) {
    yunikEngine::ProjectManager::init();

    auto w = yunikEngine::Window::create();
    w->render();

    yunikEngine::ProjectManager::deinit();
    return 0;
}
