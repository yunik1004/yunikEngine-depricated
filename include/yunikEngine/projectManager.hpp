#pragma once

#include "audio.hpp"
#include "window.hpp"

namespace yunikEngine {
    bool init (void) {
        if (!Window::init()) {
            return false;
        }
        if (!Audio::init()) {
            Window::deinit();
            return false;
        }
        return true;
    }

    void deinit (void) {
        Audio::deinit();
        Window::deinit();
    }
}
