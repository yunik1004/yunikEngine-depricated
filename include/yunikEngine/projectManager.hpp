#pragma once

#include "audio.hpp"
#include "window.hpp"

namespace yunikEngine {
    class ProjectManager {
        /***************************** PUBLIC *********************************/
        public:
        static bool init (void) {
            if (!Window::init()) {
                return false;
            }
            if (!Audio::init()) {
                Window::deinit();
                return false;
            }
            return true;
        }

        static void deinit (void) {
            Audio::deinit();
            Window::deinit();
        }

        /**************************** PRIVATE *********************************/
        private:
        ProjectManager (void) {}
        ~ProjectManager (void) {}
    };
}
