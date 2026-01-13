#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/scalar_multiplication.hpp>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

// Шейдеры
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.2f, 0.5f, 0.8f, 1.0f);
}
)";

class ShaderProgram
{
  private:
    GLuint shprog;

  public:
    ShaderProgram(std::string vshader, std::string fshader)
    {
        const char *vsrc = vshader.c_str();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsrc, NULL);
        glCompileShader(vs);

        const char *fsrc = fshader.c_str();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsrc, NULL);
        glCompileShader(fs);

        shprog = glCreateProgram();
        glAttachShader(shprog, vs);
        glAttachShader(shprog, fs);
        glLinkProgram(shprog);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    ~ShaderProgram()
    {
        glDeleteProgram(shprog);
    }

    inline GLuint GetShaderProgram() { return shprog; }
};

class Mesh
{
  private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

  public:
    Mesh(std::vector<float> _vertices, std::vector<unsigned int> _indices)
    {
        vertices = _vertices;
        indices = _indices;
    }

    Mesh() {}

    inline void ClearVertices() { vertices.clear(); }
    inline void ClearIndices() { indices.clear(); }
    inline void ClearMesh() { ClearVertices(); ClearIndices(); }

    void AddVertex(float x, float y, float z)
    {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }
    inline void AddVertex(glm::vec3 vertex) { AddVertex(vertex.x, vertex.y, vertex.z); }

    void AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2)
    {
        indices.push_back(v0);
        indices.push_back(v1);
        indices.push_back(v2);
    }

    ~Mesh() {}

    inline std::vector<float> GetVertices() { return vertices; }
    inline std::vector<unsigned int> GetIndices() { return indices; }
};

class Entity
{
  private:
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scl;

    GLuint VAO, VBO, EBO;

    Mesh *mesh = nullptr;

    void genbuffs()
    {
        if (!mesh) return;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        //glGenBuffers(1, &VBO_uvs);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->GetVertices().size() * sizeof(float), mesh->GetVertices().data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        /*glBindBuffer(GL_ARRAY_BUFFER, VBO_uvs);
        glBufferData(GL_ARRAY_BUFFER, mesh->get_uvs().size() * sizeof(float), mesh->get_uvs().data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);*/

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndices().size() * sizeof(unsigned int), mesh->GetIndices().data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void delbuffs()
    {
        if (!mesh) return;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        //glDeleteBuffers(1, &VBO_uvs);
        glDeleteBuffers(1, &EBO);
    }

  public:
    Entity() {}
    ~Entity() { delbuffs(); }

    inline glm::vec3 GetPosition() { return pos; }
    inline glm::vec3 GetRotation() { return rot; }
    inline glm::vec3 GetScale() { return scl; }

    inline Mesh *GetMesh() { return mesh; }

    inline void SetPosition(glm::vec3 v) { pos = v; }
    inline void SetRotation(glm::vec3 v) { rot = v; }
    inline void SetScale(glm::vec3 v) { scl = v; }

    inline void SetMesh(Mesh *m)
    {
        mesh = m;

        delbuffs();
        genbuffs();
    }

    void render(ShaderProgram *sp, glm::mat4 *view, glm::mat4 *projection)
    {
        if (!mesh) return;

        glUseProgram(sp->GetShaderProgram());

        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(*view));

        glm::mat4 model = glm::mat4(1);

        model = glm::translate(model, pos);
        model = glm::rotate(model, rot.x, {1, 0, 0});
        model = glm::rotate(model, rot.y, {0, 1, 0});
        model = glm::rotate(model, rot.z, {0, 0, 1});
        model = glm::scale(model, scl);
        
        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        /*// Активируем текстуру (биндим)
        glActiveTexture(GL_TEXTURE0); // Текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, texture->get_texture());

        // Указываем шейдеру, какой юнит использовать
        glUniform1i(glGetUniformLocation(shader->get_shader_program(), "texture"), 0);*/

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh->GetIndices().size(), GL_UNSIGNED_INT, 0);
    }
};

class Camera
{
  private:
    glm::vec3 pos;
    glm::vec3 rot;
    float neardist = 0.1;
    float fardist = 1000;
    float fov = glm::radians(60.0f);

    glm::vec3 front, right, up;
    glm::mat4 mview;

    void updateCache()
    {
        glm::quat q = glm::quat(rot);
        front = q * glm::vec3(0, 0, -1.0f);
        right = q * glm::vec3(1.0f, 0, 0);
        up = q * glm::vec3(0, 1.0f, 0);
        mview = glm::lookAt(pos, pos + front, up);
    }
    
  public:
    Camera(float _fov, float _neardist, float _fardist)
    {
        fov = _fov;
        neardist = _neardist;
        fardist = _fardist;
        updateCache();
    }

    Camera(float _neardist, float _fardist)
    {
        neardist = _neardist;
        fardist = _fardist;
        updateCache();
    }

    Camera(float _fov) { fov = _fov; updateCache(); }

    Camera() { updateCache(); }

    inline glm::vec3 GetPosition() { return pos; }
    inline glm::vec3 GetRotation() { return rot; }
    inline float GetNearDistance() { return neardist; }
    inline float GetFarDistance() { return fardist; }
    inline float GetFOV() { return fov; }

    inline void SetPosition(glm::vec3 v) { pos = v; updateCache(); }
    inline void SetRotation(glm::vec3 v) { rot = v; updateCache(); }
    inline void SetNearDistance(float _neardist) { neardist = _neardist; }
    inline void SetFarDistance(float _fardist) { fardist = _fardist; }
    inline void SetFOV(float _fov) { fov = _fov; }

    inline glm::vec3 GetFront() { return front; };
    inline glm::vec3 GetRight() { return right; };
    inline glm::vec3 GetUp() { return up; };

    inline glm::mat4 GetViewMatrix() { return mview; }

    inline glm::mat4 GetProjectionMatrix(unsigned int screen_width, unsigned int screen_height)
    { return glm::perspective(fov, (float)screen_width / (float)screen_height, neardist, fardist); }
};

int initOpenGL(GLFWwindow **window);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 700;
const unsigned int FPS = 60;
const float MOUSE_SENSITIVITY = 0.1;
const float GRAVITY = 9.80665f;

int main()
{
    GLFWwindow *window;
    int ret = initOpenGL(&window);
    if (ret != 0) return ret;
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glEnable(GL_DEPTH_TEST);
    
    ShaderProgram sp(vertexShaderSource, fragmentShaderSource);

    Camera cam = Camera();
    
    Mesh tri = Mesh();
    tri.AddVertex(0.0, 0.0, 0.0);
    tri.AddVertex(0.5, 1.0, 0.0);
    tri.AddVertex(1.0, 0.0, 0.0);
    tri.AddTriangle(0, 1, 2);

    Entity e = Entity();
    e.SetMesh(&tri);
    e.SetPosition({0.0, 0.0, -5.0});
    e.SetScale({4.0, 4.0, 4.0});

    float lastX = SCREEN_WIDTH / 2, lastY = SCREEN_HEIGHT / 2;

    glm::vec3 vel = glm::vec3(0.0f);
    float mass = 1;

    double prev_time = glfwGetTime();
    double start_time = prev_time;
    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime() - prev_time;
        double elapsed_time = glfwGetTime() - start_time;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        if (delta >= 1.0f / FPS)
        {
            prev_time = glfwGetTime();

            // ===== CONTROLS =====

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + cam.GetFront() * glm::vec3(3.0 * delta));
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - cam.GetFront() * glm::vec3(3.0 * delta));
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - cam.GetRight() * glm::vec3(3.0 * delta));
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + cam.GetRight() * glm::vec3(3.0 * delta));

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            glm::vec3 r = cam.GetRotation();

            r.y -= glm::radians((mouseX - lastX) * MOUSE_SENSITIVITY);
            r.x -= glm::radians((mouseY - lastY) * MOUSE_SENSITIVITY);
            if (glm::degrees(r.x) > 89.0f) r.x = glm::radians(89.0f);
            if (glm::degrees(r.x) < -89.0f) r.x = glm::radians(-89.0f);
            r.y = fmod(r.y, 360);

            cam.SetRotation(r);

            lastX = mouseX;
            lastY = mouseY;

            // ===== MAIN =====
            
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            //e.SetRotation(e.GetRotation() + glm::vec3(0, 0, glm::radians(0.1f)));
            glm::vec3 force = glm::vec3(0.0f);
            if (elapsed_time >= 5.0f && elapsed_time <= 10.0f) force += glm::vec3(0, 0, -1.0f);

            if (glm::length(vel) > 0.0f) force += -glm::normalize(vel) * 0.05f * mass * GRAVITY;

            glm::vec3 acc = force / mass;
            vel += acc * delta;
            e.SetPosition(e.GetPosition() + vel * delta);
            std::cout << e.GetPosition().x << " " << e.GetPosition().y << " " << e.GetPosition().z << std::endl;

            glm::mat4 view = cam.GetViewMatrix();
            glm::mat4 proj = cam.GetProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);
            e.render(&sp, &view, &proj);
            
            // Обмен буферов
            glfwSwapBuffers(window);

        }
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

int initOpenGL(GLFWwindow **window)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *w = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL App", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return 1;
    }

    *window = w;

    return 0;
}