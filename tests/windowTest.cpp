#include <iostream>
#include <yunikEngine/window.hpp>

int main (void) {
    std::cout << "hello" << std::endl;
    auto window = yunikEngine::Window::getInstance();
    return 0;
}
