#pragma once

#include <cstdio>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "math.hpp"
#include "projectManager.hpp"
#include "scene.hpp"

namespace yunikEngine {
    class Window {
        /***************************** PUBLIC *********************************/
        public:
        static void setGLVersion (int major, int minor) {
            gl_version_major = major;
            gl_version_minor = minor;
        }

        static void getGLVersion (int* major, int* minor) {
            *major = gl_version_major;
            *minor = gl_version_minor;
        }

        static void getMonitorSize (int* width, int* height) {
            auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            *width = mode->width;
            *height = mode->height;
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
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);

            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            /* Anti-aliasing */
            glfwWindowHint(GLFW_SAMPLES, 4);

            return true;
        }

        static void deinit (void) {
            glfwTerminate();
        }

        static Window* create (void) {
            auto newWindow = new Window();
            if (!newWindow->isValid) {
                delete newWindow;
                return nullptr;
            }
            return newWindow;
        }

        void destroy (void) {
            delete this;
        }

        void setTitle (const char* title) {
            glfwSetWindowTitle(window, title);
        }

        void getSize (int* width, int* height) {
            glfwGetWindowSize(window, width, height);
        }

        void setSize (int width, int height) {
            default_window_width = width;
            default_window_height = height;
            glfwSetWindowSize(window, width, height);
        }

        void setPos (int xpos, int ypos) {
            glfwSetWindowPos(window, xpos, ypos);
        }

        void setResizability (bool isResizable) {
            glfwSetWindowAttrib(window, GLFW_RESIZABLE, isResizable);
        }

        void setAspectRatio (int w, int h) {
            glfwSetWindowAspectRatio(window, w, h);
        }

        void disableAspectRatio (void) {
            glfwSetWindowAspectRatio(window, GLFW_DONT_CARE, GLFW_DONT_CARE);
        }

        void setViewportFullWindow (bool isViewportFull) {
            if (isViewportFull) {
                glfwSetWindowSizeCallback(window, &windowSizeCallback_ViewportFull);
            } else {
                glfwSetWindowSizeCallback(window, &windowSizeCallback_ViewportNotFull);
            }
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
            /* Create window */
            window = glfwCreateWindow(default_window_width, default_window_height, "Hello world", nullptr, nullptr);
            if (window == nullptr) {
                fprintf(stderr, "GLFW Error: Failed to create window\n");
                return;
            }

            /* Resizability */
            setResizability(false);

            /* Set Aspect ratio */
            setAspectRatio(default_window_width, default_window_height);

            /* Set window position */
            int monitor_width, monitor_height;
            getMonitorSize(&monitor_width, &monitor_height);
			const auto window_width_diff = monitor_width - default_window_width;
			const auto window_height_diff = monitor_height - default_window_height;
            setPos(round(window_width_diff / 2.0), round(window_height_diff / 2.0));

            glfwMakeContextCurrent(window);

            /* FPS limit: 60 */
            glfwSwapInterval(1);

            /* Viewport setting & Window size change callback */
            setViewportFullWindow(false);

            /* Initialize glew */
            GLenum errorCode = glewInit();
            if (errorCode != GLEW_OK) {
                fprintf(stderr, "GLEW Error: Failed to initialize. %s\n", glewGetErrorString(errorCode));
                glfwDestroyWindow(window);
                return;
            }

            /* OpenGL version check */
            int gl_major, gl_minor;
            getGLVersion(&gl_major, &gl_minor);
            char gl_version[17];
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

        ~Window (void) {
            if (window != nullptr) {
                glfwDestroyWindow(window);
            }
            delete scene;
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

        static void windowSizeCallback_ViewportFull (GLFWwindow* window, int w, int h) {
            glViewport(0, 0, w, h);
        }

        static void windowSizeCallback_ViewportNotFull (GLFWwindow* window, int w, int h) {
            GLfloat widthFactor = (GLfloat)w / (GLfloat)default_window_width;
            GLfloat heightFactor = (GLfloat)h / (GLfloat)default_window_height;

            GLfloat sizeFactor = widthFactor < heightFactor ? widthFactor : heightFactor;

            GLint modifiedWidth =  (int) ceil(default_window_width * sizeFactor) + 1;
            GLint modifiedHeight = (int) ceil(default_window_height * sizeFactor) + 1;

            glViewport((int)floor((w - modifiedWidth) / 2.0), (int)floor((h - modifiedHeight) / 2.0), modifiedWidth, modifiedHeight);
        }

        static int gl_version_major;
        static int gl_version_minor;

        int default_window_width = 1024;
        int default_window_height = 768;

        GLFWwindow* window = nullptr;
        Scene* scene = nullptr;

        bool isValid = false;
    };

    /************************** INITIALIZATION ********************************/
    int Window::gl_version_major = 4;
    int Window::gl_version_minor = 4;
}
