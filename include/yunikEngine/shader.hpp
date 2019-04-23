#pragma once

#include <cstdio>
#include <string>
#include <GL/glew.h>
#include "window.hpp"

namespace yunikEngine {
    /**************************************************************************/
    /*                               ShaderType                               */
    /**************************************************************************/
    enum class ShaderType : GLenum {
        VERTEX = GL_VERTEX_SHADER,
        TESS_CONTROL = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        COMPUTE = GL_COMPUTE_SHADER
    };

    /**************************************************************************/
    /*                                 Shader                                 */
    /**************************************************************************/
    class Shader {
        /***************************** PUBLIC *********************************/
        public:
        static Shader* create (const char* shaderSrc, const ShaderType shaderType) {
            auto newShader = new Shader(shaderSrc, static_cast<GLenum>(shaderType));
            if (!newShader->isValid) {
                newShader->destroy();
                return nullptr;
            }
            return newShader;
        }

        void destroy (void) {
            delete this;
        }

        GLuint getShader (void) {
            return shader;
        }

        /**************************** PRIVATE *********************************/
        private:
        Shader (const char* shaderSrc, const GLenum shaderType) {
            shader = glCreateShader(shaderType);
            if (shader == 0) {
                return;
            }
            glShaderSource(shader, 1, &shaderSrc, nullptr);
            glCompileShader(shader);

            // Compile check
            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                GLchar* errorLog = new GLchar[maxLength];
                glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
                fprintf(stderr, "Error: Shader compilation failed. %s\n", errorLog);
                delete[] errorLog;
                glDeleteShader(shader);
                return;
            }

            isValid = true;
        }

        ~Shader (void) {
            glDeleteShader(shader);
        }

        GLuint shader = 0;

        bool isValid = false;
    };

    /**************************************************************************/
    /*                             Shader Program                             */
    /**************************************************************************/
    class ShaderProgram {
        /***************************** PUBLIC *********************************/
        public:
        static ShaderProgram* create (void) {
            auto newShaderProgram = new ShaderProgram();
            if (!newShaderProgram->isValid) {
                newShaderProgram->destroy();
                return nullptr;
            }
            return newShaderProgram;
        }

        void destroy (void) {
            delete this;
        }

        void attachShader (Shader* shader) {
            glAttachShader(program, shader->getShader());
        }

        bool compile (void) {
            glLinkProgram(program);
            
            GLint isCompiled = 0;
            glGetShaderiv(program, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                GLchar* errorLog = new GLchar[maxLength];
                glGetShaderInfoLog(program, maxLength, &maxLength, errorLog);
                fprintf(stderr, "Error: ShaderProgram compilation failed. %s\n", errorLog);
                delete[] errorLog;
                return false;
            }
            return true;
        }

        void use (void) {
            glUseProgram(program);
        }

        void setInt (const char* name, int value) {
            glUniform1i(glGetUniformLocation(program, name), value);
        }

        void setFloat (const char* name, float value) {
            glUniform1f(glGetUniformLocation(program, name), value);
        }

        /**************************** PRIVATE *********************************/
        private:
        ShaderProgram (void) {
            program = glCreateProgram();
            if (program != 0) {
                isValid = true;
            }
        }

        ~ShaderProgram(void) {
            glDeleteProgram(program);
        }

        GLuint program;

        bool isValid = false;
    };

    /**************************************************************************/
    /*                            Shader examples                             */
    /**************************************************************************/
    namespace example {
        char* simpleVertexShader (void) {
            char* glslCore = Window::getGLSLCore();
            std::string code = "\
                layout (location = 0) in vec3 vertex;\
                \
                uniform mat4 aModel_view;\
                uniform mat4 aProjection;\
                \
                void main (void) {\
                    gl_Position = aProjection * aModel_view * vec4(vertex, 1.0);\
                }\
            ";
            std::string shader_str = std::string(glslCore) + code;
            delete glslCore;
            int shaderSize = shader_str.size();
            char* shader = new char[shaderSize + 1];
            strcpy_s(shader, shaderSize + 1, shader_str.c_str());
            return shader;
        }

        char* simpleFragmentShader (void) {
            char* glslCore = Window::getGLSLCore();
            std::string code = "\
                out vec3 color;\
                \
                void main (void) {\
                    color = vec3(1.0, 1.0, 1.0);\
                }\
            ";
            std::string shader_str = std::string(glslCore) + code;
            delete glslCore;
            int shaderSize = shader_str.size();
            char* shader = new char[shaderSize + 1];
            strcpy_s(shader, shaderSize + 1, shader_str.c_str());
            return shader;
        }
    }
}
