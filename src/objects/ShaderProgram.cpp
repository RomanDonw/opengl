#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram() {}
ShaderProgram::~ShaderProgram()
{
    DeleteVertexShader();
    DeleteFragmentShader();

    DeleteShaderProgram();
}

bool ShaderProgram::HasShaderProgram() { return this->hasShaderProgram; }
bool ShaderProgram::HasVertexShader() { return this->hasVertexShader; }
bool ShaderProgram::HasFragmentShader() { return this->hasFragmentShader; }

bool ShaderProgram::IsVertexShaderCompiled() { return vertexShaderCompiled; }
bool ShaderProgram::IsFragmentShaderCompiled() { return fragmentShaderCompiled; }

bool ShaderProgram::DeleteShaderProgram()
{
    if (!HasShaderProgram()) return false;
    glDeleteProgram(shaderProgram);
    hasShaderProgram = false;
    return true;
}

bool ShaderProgram::DeleteVertexShader()
{
    if (!HasVertexShader()) return false;
    glDeleteShader(vertexShader);
    hasVertexShader = false;
    vertexShaderCompiled = false;
    return true;
}

bool ShaderProgram::DeleteFragmentShader()
{
    if (!HasFragmentShader()) return false;
    glDeleteShader(fragmentShader);
    hasFragmentShader = false;
    fragmentShaderCompiled = false;
    return true;
}

bool ShaderProgram::LoadVertexShader(std::string source)
{
    if (HasVertexShader()) return false;

    const char *source_ptr = source.c_str();
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &source_ptr, NULL);
    hasVertexShader = true;

    return true;
}

bool ShaderProgram::LoadFragmentShader(std::string source)
{
    if (HasFragmentShader()) return false;

    const char *source_ptr = source.c_str();
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &source_ptr, NULL);
    hasFragmentShader = true;

    return true;
}

bool ShaderProgram::CompileVertexShader(std::string *errorlog)
{
    if (errorlog) *errorlog = "";
    if (!HasVertexShader() || IsVertexShaderCompiled()) { return false; }

    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
        
        GLchar *log = (GLchar *)malloc(length * sizeof(GLchar));
        if (!log) return false;

        glGetShaderInfoLog(vertexShader, length, &length, log);

        if (errorlog) *errorlog = std::string(log);

        free(log);
        DeleteVertexShader(); // is neccessary to need to delete the shader when compiling error occured?... i think yes.

        return false;
    }
    vertexShaderCompiled = true;

    return true;
}

bool ShaderProgram::CompileFragmentShader(std::string *errorlog)
{
    if (errorlog) *errorlog = "";
    if (!HasFragmentShader() || IsFragmentShaderCompiled()) { return false; }

    glCompileShader(fragmentShader);

    GLint success;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
        
        GLchar *log = (GLchar *)malloc(length * sizeof(GLchar));
        if (!log) return false;

        glGetShaderInfoLog(fragmentShader, length, &length, log);

        if (errorlog) *errorlog = std::string(log);

        free(log);
        DeleteFragmentShader(); // is neccessary to need to delete the shader when compiling error occured?... i think yes.

        return false;
    }
    fragmentShaderCompiled = true;

    return true;
}

bool ShaderProgram::LinkShaderProgram(std::string *errorlog)
{
    if (errorlog) *errorlog = "";
    if (HasShaderProgram() || !HasVertexShader() || !HasFragmentShader()) return false;

    hasShaderProgram = true;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);

        GLchar *log = (GLchar *)malloc(length * sizeof(GLchar));
        if (!log) return false;

        glGetProgramInfoLog(shaderProgram, length, &length, log);

        if (errorlog) *errorlog = log;

        free(log);

        return false;
    }

    return true;
}

bool ShaderProgram::UseThisProgram()
{
    if (!HasShaderProgram()) return false;

    glUseProgram(shaderProgram);

    return true;
}

bool ShaderProgram::HasUniform(std::string name)
{
    if (!HasShaderProgram()) return false;

    return glGetUniformLocation(shaderProgram, name.c_str()) != -1;
}

bool ShaderProgram::SetUniformInteger(std::string name, int value)
{
    if (!HasUniform(name)) return false;

    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);

    return true;
}

bool ShaderProgram::SetUniformFloat(std::string name, float value)
{
    if (!HasUniform(name)) return false;

    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);

    return true;
}

bool ShaderProgram::SetUniformVector3(std::string name, glm::vec3 value)
{
    if (!HasUniform(name)) return false;

    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));

    return true;
}

bool ShaderProgram::SetUniformVector4(std::string name, glm::vec4 value)
{
    if (!HasUniform(name)) return false;

    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));

    return true;
}

bool ShaderProgram::SetUniformMatrix4x4(std::string name, glm::mat4 value)
{
    if (!HasUniform(name)) return false;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));

    return true;
}