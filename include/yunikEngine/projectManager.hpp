#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace yunikEngine {
    class ProjectManager {
        /***************************** PUBLIC *********************************/
        public:
        static void setGLVersion (int major, int minor) {
            opengl_version_major = major;
            opengl_version_minor = minor;
        }

        static void getGLVersion (int* major, int* minor) {
            *major = opengl_version_major;
            *minor = opengl_version_minor;
        }

        static bool init (void) {
            /* Initialize glfw */
            glfwSetErrorCallback([](int errorCode, const char* errorDescription) {
                fprintf(stderr, "GLFW Error: %s\n", errorDescription);
            });

            if (!glfwInit()) {
                fprintf(stderr, "GLFW Error: Failed to initialize\n");
                return false;
            }

            /* OpenGL version */
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version_major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version_minor);

            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            return true;
        }

        static void deinit (void) {
            glfwTerminate();
        }

        static ProjectManager* getInstance (void) {
            if (instance == nullptr) {
                instance = new ProjectManager();
            }
            return instance;
        }

        static void purgeInstance (void) {
            delete instance;
        }

        static void getMonitorSize (int* width, int* height) {
            auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            *width = mode->width;
            *height = mode->height;
        }

        /**************************** PRIVATE *********************************/
        private:
        ProjectManager (void) {}
        ~ProjectManager (void) {}

        static int opengl_version_major;
        static int opengl_version_minor;

        static ProjectManager* instance;
    };

    /************************** INITIALIZATION ********************************/
    int ProjectManager::opengl_version_major = 4;
    int ProjectManager::opengl_version_minor = 4;

    ProjectManager* ProjectManager::instance = nullptr;
}
