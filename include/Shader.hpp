#pragma once

#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <plog/Log.h>

class Shader {
    GLuint _programId{0};
    bool _isLinked{false};

    static bool checkCompileStatus(GLuint shader, const std::string& shaderTypeStr) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> infoLog(logLength);
            glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
            PLOG_ERROR << "Error::Shader::" << shaderTypeStr << "::Compilation failed\n"
                       << infoLog.data();
        }
        return static_cast<bool>(success);
    }

    static GLuint compileShader(GLenum type, const char* sourceCode) {
        GLuint shader = glCreateShader(type);
        if (shader == 0) {
            PLOG_ERROR << "Error::Shader::" << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "::Creation failed";
            return 0;
        }

        glShaderSource(shader, 1, &sourceCode, nullptr);
        glCompileShader(shader);

        if (!checkCompileStatus(shader, type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")) {
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    static bool checkLinkStatus(GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> infoLog(logLength);
            glGetProgramInfoLog(program, logLength, nullptr, infoLog.data());
            PLOG_ERROR << "Error::Shader::Program::Linking failed\n"
                       << infoLog.data();
        }
        return static_cast<bool>(success);
    }

public:
    Shader() {}

    ~Shader() {
        if (_programId != 0) {
            glDeleteProgram(_programId);
            _programId = 0;
        }
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept
        : _programId(other._programId), _isLinked(other._isLinked) {
        other._programId = 0;
        other._isLinked = false;
    }

    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (_programId != 0) {
                glDeleteShader(_programId);
            }
            _programId = other._programId;
            _isLinked = other._isLinked;
            other._programId = 0;
            other._isLinked = false;
        }
        return *this;
    }

    bool loadFromStrings(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
        if (_programId != 0) {
            glDeleteProgram(_programId);
            _programId = 0;
            _isLinked = false;
        }

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
        if (vertexShader == 0) return false;

        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
        if (fragmentShader == 0) {
            glDeleteShader(vertexShader);
            return false;
        }

        _programId = glCreateProgram();
        if (_programId == 0) {
            PLOG_ERROR << "Error::Shader::Program::Creation failed";
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        glAttachShader(_programId, vertexShader);
        glAttachShader(_programId, fragmentShader);
        glLinkProgram(_programId);

        if (!checkLinkStatus(_programId)) {
            glDeleteProgram(_programId);
            _programId = 0;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        _isLinked = true;
        return true;
    }

    bool loadFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        std::ifstream vShaderFile(vertexShaderPath);
        std::ifstream fShaderFile(fragmentShaderPath);

        if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
            PLOG_ERROR << "Error::Shader::File reading failed. Vertex Shader Path: "
                       << vertexShaderPath << " Fragment Shader Path: " << fragmentShaderPath;
            return false;
        }

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        return loadFromStrings(vShaderStream.str(), fShaderStream.str());
    }

    void use() const {
        if (_programId != 0 && _isLinked) {
            glUseProgram(_programId);
        } else {
            PLOG_ERROR << "Warning::Shader::Attempted to use unlinked or invalid shader program";
        }
    }

    static void unuse() {
        glUseProgram(0);
    }

    GLuint getProgramId() const {
        return _programId;
    }

    bool isLinked() const {
        return _isLinked;
    }

    GLint getUniformLocation(const std::string& name) const {
        return _isLinked ? glGetUniformLocation(_programId, name.c_str()) : -1;
    }

    GLint getAttribLocation(const std::string& name) const {
        return _isLinked ? glGetAttribLocation(_programId, name.c_str()) : -1;
    }

    void setBool(const std::string& name, bool value) const {
        if (_isLinked) {
            glUniform1i(glGetUniformLocation(_programId, name.c_str()), static_cast<int>(value));
        }
    }

    void setInt(const std::string& name, int value) const {
        if (_isLinked) {
            glUniform1i(glGetUniformLocation(_programId, name.c_str()), value);
        }
    }

    void setFloat(const std::string& name, float value) const {
        if (_isLinked) {
            glUniform1f(glGetUniformLocation(_programId, name.c_str()), value);
        }
    }

    void setVec3(const std::string& name, const glm::vec3& vec3) const {
        if (_isLinked) {
            glUniform3fv(glGetUniformLocation(_programId, name.c_str()), 1, glm::value_ptr(vec3));
        }
    }

    void setVec4(const std::string& name, const glm::vec4& vec4) const {
        if (_isLinked) {
            glUniform4fv(glGetUniformLocation(_programId, name.c_str()), 1, glm::value_ptr(vec4));
        }
    }

    void setMat4(const std::string& name, glm::mat4 value) const {
        if (_isLinked) {
            glUniformMatrix4fv(glGetUniformLocation(_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        }
    }
};