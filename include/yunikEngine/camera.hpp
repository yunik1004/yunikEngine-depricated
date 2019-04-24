#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace yunikEngine {
    class Camera {
        /***************************** PUBLIC *********************************/
        public:
        static Camera* create (bool isOrtho, float width, float height) {
            auto cam = new Camera(isOrtho, width, height);
            return cam;
        }

        void destroy (void) {
            delete this;
        }

        void setFov (float fov) {
            aFov = fov;
            calculateProjMatrix();
        }

        void setViewport (float width_left, float width_right, float height_bottom, float height_top) {
            aViewportWidth_left = width_left;
            aViewportWidth_right = width_right;
            aViewportHeight_bottom = height_bottom;
            aViewportHeight_top = height_top;
            calculateProjMatrix();
        }

        void setViewport (float width, float height) {
            setViewport(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0);
        }

        void setDepth (float zNear, float zFar) {
            aZNear = zNear;
            aZFar = zFar;
            calculateProjMatrix();
        }

        void setOrtho (bool isOrtho) {
            isOrthographic = isOrtho;
            calculateProjMatrix();
        }

        void setViewMatrix (glm::vec3 pos, glm::vec3 at, glm::vec3 up) {
            viewMatrix = glm::lookAt(pos, at, up);
        }

        /**************************** PRIVATE *********************************/
        private:
        Camera (bool isOrtho, float width, float height) {
            viewMatrix = glm::lookAt(default_pos, default_at, default_up);
            isOrthographic = isOrtho;
            if (isOrthographic) {
                aZNear = 0.0f;
                aZFar = 100.0f;
                setViewport(0.0, width, 0.0, height);
            } else {
                aZNear = 0.1f;
                aZFar = 100.0f;
                setViewport(width, height);
            }
        }

        ~Camera (void) {}

        void calculateProjMatrix (void) {
            if (isOrthographic) {
                projMatrix = glm::ortho(aViewportWidth_left, aViewportWidth_right, aViewportHeight_bottom, aViewportHeight_top, aZNear, aZFar);
            } else {
                projMatrix = glm::perspective(glm::radians(aFov), (aViewportWidth_right - aViewportWidth_left) / (aViewportHeight_top - aViewportHeight_bottom), aZNear, aZFar);
            }
        }

        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;

        float aFov = 60.0f;
        float aViewportWidth_left;
        float aViewportWidth_right;
        float aViewportHeight_bottom;
        float aViewportHeight_top;
        float aZNear;
        float aZFar;

        bool isOrthographic;

        const glm::vec3 default_pos = glm::vec3(0.0, 0.0, 0.0);
        const glm::vec3 default_at = glm::vec3(0.0, 0.0, -1.0);
        const glm::vec3 default_up = glm::vec3(0.0, 1.0, 0.0);
    };
}
