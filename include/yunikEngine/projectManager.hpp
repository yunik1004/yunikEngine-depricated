#pragma once

#include "window.hpp"

namespace yunikEngine {
    class ProjectManager {
        /***************************** PUBLIC *********************************/
        public:
        static ProjectManager* getInstance (void) {
            if (instance == nullptr) {
                instance = new ProjectManager();
            }
            return instance;
        }

        static void purgeInstance (void) {
            delete instance;
        }

        static void getWindow (void) {
            return;
        }

        /**************************** PRIVATE *********************************/
        private:
        ProjectManager (void) {
            // init
        }

        ~ProjectManager (void) {{
            // deinit
        }}

        static ProjectManager* instance;
        
    };

    /************************** INITIALIZATION ********************************/
    ProjectManager* ProjectManager::instance = nullptr;
}
