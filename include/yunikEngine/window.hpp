#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "projectManager.hpp"
#include "scene.hpp"

namespace yunikEngine {
    class Window {
        /***************************** PUBLIC *********************************/
        public:
        static Window* create (void) {
            auto newWindow = new Window();
            if (!newWindow->isValid) {
                delete newWindow;
                return nullptr;
            }
            return newWindow;
        }

        ~Window (void) {
            if (window != nullptr) {
                glfwDestroyWindow(window);
            }
            delete scene;
        }

        void setScene (Scene* newScene) {
            if (scene != nullptr) {
                delete scene;
            }
            scene = newScene;
        }

        void render (void) {
            while (!glfwWindowShouldClose(window)) {
                updateScene();
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }

        /**************************** PRIVATE *********************************/
        private:
        Window (void) {
            /* Anti-aliasing */
            glfwWindowHint(GLFW_SAMPLES, 4);
            
            /* Create window */
            window = glfwCreateWindow(1024, 768, "Hello world", nullptr, nullptr);
            if (window == nullptr) {
                fprintf(stderr, "GLFW Error: Failed to create window\n");
                return;
            }

            glfwMakeContextCurrent(window);

            /* FPS limit: 60 */
            glfwSwapInterval(1);

            /* Initialize glew */
            GLenum errorCode = glewInit();
            if (errorCode != GLEW_OK) {
                fprintf(stderr, "GLEW Error: Failed to initialize. %s\n", glewGetErrorString(errorCode));
                glfwDestroyWindow(window);
                return;
            }

            /* OpenGL version check */
            int gl_major, gl_minor;
            ProjectManager::getGLVersion(gl_major, gl_minor);
            char gl_version[15];
            sprintf(gl_version, "GL_VERSION_%d_%d", gl_major, gl_minor);
            if (!glewIsSupported(gl_version)) {
                fprintf(stderr, "OpenGL Error: %s is not available\n", gl_version);
                glfwDestroyWindow(window);
                return;
            }

            /* Set OpenGL options */
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            isValid = true;
        }

        void updateScene (void) {
            if (scene == nullptr) {
                return;
            }
            Scene* nextScene = scene->update();
            if (nextScene != scene) {
                delete scene;
                scene = nextScene;
            }
        }

        GLFWwindow* window = nullptr;
        Scene* scene = nullptr;

        bool isValid = false;
    };
}
