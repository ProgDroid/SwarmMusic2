/**
 * Slightly modified Shader class from http://learnopengl.com
 */

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader
{
private:
    unsigned int id;

    void checkErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type == "PROGRAM") {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_LINKING_ERROR\n" << infoLog << "\n-----------------------------" << std::endl;
            }
            return;
        }

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILING_ERROR of type: " << type << "\n" << infoLog << "\n-----------------------------" << std::endl;
        }
    }

public:
    unsigned int getId() {
        return id;
    }

    void setId(unsigned int value) {
        id = value;
    }

    Shader(const char* vertexPath, const char* fragmentPath) {
        // read shaders from files
        std::string vertexCode, fragmentCode;
        std::ifstream vertexShaderFile, fragmentShaderFile;

        // to ensure ifstream objects can throw exceptions
        vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vertexShaderFile.open(vertexPath);
            fragmentShaderFile.open(fragmentPath);

            std::stringstream vertexShaderStream, fragmentShaderStream;

            vertexShaderStream << vertexShaderFile.rdbuf();
            fragmentShaderStream << fragmentShaderFile.rdbuf();

            vertexShaderFile.close();
            fragmentShaderFile.close();

            vertexCode   = vertexShaderStream.str();
            fragmentCode = fragmentShaderStream.str();
        } catch (std::ifstream::failure e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }

        const char* vertexShaderCode   = vertexCode.c_str();
        const char* fragmentShaderCode = fragmentCode.c_str();

        // compile shaders
        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexShaderCode, NULL);
        glCompileShader(vertex);
        checkErrors(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
        glCompileShader(fragment);
        checkErrors(fragment, "FRAGMENT");

        // link shaders
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkErrors(id, "PROGRAM");

        // delete shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() {
        glUseProgram(id);
    }

    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }
    void setMat4(const std::string &name, glm::mat4 value) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setVec3(const std::string &name, glm::vec3 value) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
    }
};

#endif