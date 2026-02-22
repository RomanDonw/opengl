#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include "../opengl.hpp"
#include "../glm.hpp"

#include <string>

class ShaderProgram
{
  private:
    GLuint shaderProgram;
    bool hasShaderProgram = false;

    GLuint vertexShader;
    bool hasVertexShader = false;
    bool vertexShaderCompiled = false;

    GLuint fragmentShader;
    bool hasFragmentShader = false;
    bool fragmentShaderCompiled = false;

  public:
    ShaderProgram();
    ~ShaderProgram();

    bool HasShaderProgram();
    bool HasVertexShader();
    bool HasFragmentShader();

    bool IsVertexShaderCompiled();
    bool IsFragmentShaderCompiled();

    bool DeleteShaderProgram();
    bool DeleteVertexShader();
    bool DeleteFragmentShader();

    bool LoadVertexShader(std::string source);
    bool LoadFragmentShader(std::string source);

    bool CompileVertexShader(std::string *errorlog = nullptr);
    bool CompileFragmentShader(std::string *errorlog = nullptr);

    bool LinkShaderProgram(std::string *errorlog = nullptr);
    bool UseThisProgram();

    bool HasUniform(std::string name);
    bool SetUniformInteger(std::string name, int value);
    bool SetUniformFloat(std::string name, float value);
    bool SetUniformVector3(std::string name, glm::vec3 value);
    bool SetUniformVector4(std::string name, glm::vec4 value);
    bool SetUniformMatrix4x4(std::string name, glm::mat4 value);
};


#endif