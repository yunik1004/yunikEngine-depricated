#pragma once

#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "projectManager.hpp"

namespace yunikEngine {
    class Window {
        /***************************** PUBLIC *********************************/
        public:
        static Window* getInstance (void) {
            if (instance == nullptr) {
                instance = new Window();
            }
            return instance;
        }

        static void purgeInstance (void) {
            delete instance;
        }

        static void setOpenGLVersion (int major, int minor) {
            opengl_version_major = major;
            opengl_version_minor = minor;
        }

        /**************************** PRIVATE *********************************/
        private:
        Window (void) {
            /* Initialize glfw */
            glfwSetErrorCallback([](int errorCode, const char* errorDescription) {
                std::cerr << "GLFW Error: " << errorDescription << std::endl;
            });

            if (!glfwInit()) {
                std::cerr << "GLFW Error: Fail to initialize" << std::endl;
                exit(EXIT_FAILURE);
            }

            /* OpenGL version */
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version_major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version_minor);

            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            /* Anti-aliasing */
            glfwWindowHint(GLFW_SAMPLES, 4);

            /* Create window */
            window = glfwCreateWindow(1024, 768, "Hello world", nullptr, nullptr);
            if (window == nullptr) {
                glfwTerminate();
                exit(EXIT_FAILURE);
            }

            glfwMakeContextCurrent(window);

            /* FPS limit: 60 */
            glfwSwapInterval(1);

            /* Initialize glew */
            GLenum errorCode = glewInit();
            if (errorCode != GLEW_OK) {
                std::cerr << "GLEW Error: Fail to initialize. " << glewGetErrorString(errorCode) << std::endl;
                glfwDestroyWindow(window);
                glfwTerminate();
                exit(EXIT_FAILURE);
            }

            /* OpenGL version check */
            std::stringstream gl_version;
            gl_version << "GL_VERSION_" << opengl_version_major << "_" << opengl_version_minor;
            const char* gl_version_char = gl_version.str().c_str();
            if (!glewIsSupported(gl_version_char)) {
                std::cerr << gl_version_char << " is not avaliable." << std::endl;
                glfwDestroyWindow(window);
                glfwTerminate();
                exit(EXIT_FAILURE);
            }

            /* Set OpenGL options */
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        ~Window (void) {
            if (window != nullptr) {
                glfwDestroyWindow(window);
            }
            
            glfwTerminate();
        }

        static Window* instance;
        static int opengl_version_major;
        static int opengl_version_minor;

        GLFWwindow* window;
    };

    /************************** INITIALIZATION ********************************/
    Window* Window::instance = nullptr;
    int Window::opengl_version_major = 4;
    int Window::opengl_version_minor = 4;
}
