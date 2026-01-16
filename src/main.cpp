#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include <cstdint>
#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <AL/al.h>

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 color;
uniform sampler2D texture;

void main()
{
    FragColor = texture2D(texture, TexCoord) * color;
}
)";

class ShaderProgram
{
  private:
    GLuint shprog = 0;

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
    std::vector<float> uvs;

    bool hasbuffers = false;
    GLuint VAO, VBO_VERTEX, VBO_UV, EBO;

    bool lockbuffers = false;
    inline void updatebuffers() { if (!lockbuffers) RegenerateBuffers(); }

  public:
    Mesh(std::vector<float> _vertices, std::vector<unsigned int> _indices, std::vector<float> _uvs)
    {
        vertices = _vertices;
        indices = _indices;
        uvs = _uvs;

        GenerateBuffers();
    }

    Mesh() {}
    ~Mesh() { DeleteBuffers(); }

    inline void ClearVertices() { vertices.clear(); DeleteBuffers(); }
    inline void ClearIndices() { indices.clear(); DeleteBuffers(); }
    inline void ClearUVs() { uvs.clear(); DeleteBuffers(); }
    inline void ClearMesh() { ClearVertices(); ClearIndices(); ClearUVs(); }

    void AddVertexWithUV(float x, float y, float z, float u, float v)
    {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        uvs.push_back(u);
        uvs.push_back(v);

        updatebuffers();
    }
    inline void AddVertexWithUV(glm::vec3 vertex, glm::vec2 uv) { AddVertexWithUV(vertex.x, vertex.y, vertex.z, uv.x, uv.y); }

    void AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2)
    {
        indices.push_back(v0);
        indices.push_back(v1);
        indices.push_back(v2);

        updatebuffers();
    }

    inline std::vector<float> GetVertices() { return vertices; }
    inline std::vector<unsigned int> GetIndices() { return indices; }
    inline size_t GetIndicesCount() { return indices.size(); }
    inline std::vector<float> GetUVs() { return uvs; }

    inline bool IsBuffersLocked() { return lockbuffers; }
    inline void SetBuffersLock(bool state) { lockbuffers = state; }
    inline void LockBuffers() { lockbuffers = true; }
    inline void UnlockBuffers() { lockbuffers = false; }

    inline bool HasBuffers() { return hasbuffers; }
    inline GLuint GetVAO() { return VAO; }

    bool GenerateBuffers()
    {
        if (hasbuffers || vertices.size() == 0 || uvs.size() == 0 || indices.size() == 0) return false;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_VERTEX);
        glGenBuffers(1, &VBO_UV);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTEX);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        hasbuffers = true;
        return true;
    }

    bool DeleteBuffers()
    {
        if (!hasbuffers) return false;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO_VERTEX);
        glDeleteBuffers(1, &VBO_UV);
        glDeleteBuffers(1, &EBO);

        hasbuffers = false;
        return true;
    }

    inline void RegenerateBuffers() { GenerateBuffers(); DeleteBuffers(); }
};

class Texture
{
  private:
    GLuint texture = 0;

  public:
    Texture() {}
    ~Texture() { DeleteTexture(); }

    inline bool HasTexture() { return glIsTexture(texture) == GL_TRUE; }
    inline GLuint GetTexture() { return texture; }

    bool DeleteTexture()
    {
        if (!HasTexture()) return false;
        glDeleteTextures(1, &texture);
        texture = 0;
        return true;
    }

    bool LoadFromUCTEXFile(std::string filename)
    {
        if (!std::filesystem::is_regular_file(filename)) return false;

        FILE *f = fopen(filename.c_str(), "rb");
        if (!f) return false;

        char sig[5];
        fread(&sig, sizeof(char), 5, f);
        if (feof(f) || strncmp(sig, "UCTEX", 5)) { fclose(f); return false; }

        uint16_t version;
        fread(&version, sizeof(uint16_t), 1, f);
        if (feof(f) || version != 0) { fclose(f); return false; }

        uint8_t type;
        fread(&type, sizeof(uint8_t), 1, f);
        if (feof(f) || (type > 3)) { fclose(f); return false; }

        uint16_t width16, height16;
        fread(&width16, sizeof(uint16_t), 1, f);
        fread(&height16, sizeof(uint16_t), 1, f);
        if (feof(f)) { fclose(f); return false; }
        uint32_t width = width16 + 1;
        uint32_t height = height16 + 1;

        DeleteTexture();

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        bool texmiss_y = false;
        std::vector<uint32_t> pixels;
        switch (type)
        {
            case 0: // RGBA (0xAABBGGRR).
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint32_t pixel;
                        fread(&pixel, sizeof(uint32_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) pixel = 0xFFFF00FF;
                            else pixel = 0xFF000000;
                        }
                        pixels.push_back(pixel);

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;

            case 1: // RGB (0xBBGGRR).
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint8_t r, g, b;
                        fread(&r, sizeof(uint8_t), 1, f);
                        fread(&g, sizeof(uint8_t), 1, f);
                        fread(&b, sizeof(uint8_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) { r = 0; g = 255; b = 0; }
                            else { r = 255; g = 255; b = 255; }
                        }
                        pixels.push_back((0xFF << 24) | (b << 16) | (g << 8) | r);

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;
            
            case 2: // 16-bit depth RGBA (1 bit alpha). (0bABBBBBGGGGGRRRRR)
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint16_t pixel;
                        fread(&pixel, sizeof(uint16_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) pixel = 0b1000001111111111;
                            else pixel = 0b1111110000000000;
                        }
                        pixels.push_back((((pixel >> 15) * 255) << 24) | ((pixel & 0b11111) << 3) | ((pixel & (0b11111 << 5)) << 6) | ((pixel & (0b11111 << 10)) << 9));

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;

            case 3: // 16-bit depth RGB (0b0BBBBBGGGGGRRRRR).
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint16_t pixel;
                        fread(&pixel, sizeof(uint16_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) pixel = 0b0111111111100000;
                            else pixel = 0b0000000000011111;
                        }
                        pixels.push_back((0xFF << 24) | ((pixel & 0b11111) << 3) | ((pixel & (0b11111 << 5)) << 6) | ((pixel & (0b11111 << 10)) << 9));

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        glBindTexture(GL_TEXTURE_2D, 0);
        fclose(f);

        return true;
    }

    bool SetTextureIntParameter(GLenum param, GLint value)
    {
        if (!HasTexture()) return false;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, param, value);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void SetDefaultParametres()
    {
        SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SetTextureIntParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        SetTextureIntParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
};

enum
{
    NoCulling = 0,
    BackFace = 1,
    FrontFace = 2,
    BothFaces = 3
} typedef FaceCullingType;

class Surface
{
  private:
    Texture *texture = nullptr;
    Mesh *mesh = nullptr;
    FaceCullingType culling = BackFace;

  public:
    Surface(Texture *t, Mesh *m, FaceCullingType c)
    {
        texture = t;
        mesh = m;
        culling = c;
    }

    Surface(Mesh *m, Texture *t, FaceCullingType c)
    {
        texture = t;
        mesh = m;
        culling = c;
    }

    Surface(Texture *t, Mesh *m)
    {
        texture = t;
        mesh = m;
    }

    Surface(Mesh *m, Texture *t)
    {
        texture = t;
        mesh = m;
    }

    ~Surface() {}

    inline Texture *GetTexture() { return texture; }
    inline Mesh *GetMesh() { return mesh; }
    inline FaceCullingType GetFaceCullingType() { return culling; }

    inline void SetTexture(Texture *t) { texture = t; }
    inline void SetMesh(Mesh *m) { mesh = m; }
    inline void SetFaceCullingType(FaceCullingType c) { culling = c; }
};

class Entity
{
  private:
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
    glm::vec3 scl = glm::vec3(0.0f);
    glm::vec4 color = glm::vec4(1.0f);

    std::vector<Surface> surfaces;

    bool enable_render = true;

  public:
    Entity() {}
    ~Entity() {}

    inline glm::vec3 GetPosition() { return pos; }
    inline glm::vec3 GetRotation() { return rot; }
    inline glm::vec3 GetScale() { return scl; }
    inline glm::vec4 GetColor() { return color; }

    inline void SetPosition(glm::vec3 v) { pos = v; }
    inline void SetRotation(glm::vec3 v) { rot = v; }
    inline void SetScale(glm::vec3 v) { scl = v; }
    inline void SetColor(glm::vec4 c) { color = c; }

    inline Surface GetSurface(size_t index) { return surfaces.at(index); }
    inline size_t GetSurfacesCount() { return surfaces.size(); }
    inline std::vector<Surface> GetSurfaces() { return surfaces; }
    inline void ClearSurfaces() { surfaces.clear(); }
    inline bool IsSurfaceExist(size_t index) { return index < surfaces.size(); }
    inline void AddSurface(Surface surface) { surfaces.push_back(surface); }
    bool SetSurface(size_t index, Surface surface)
    {
        if (!IsSurfaceExist(index)) return false;
        surfaces[index] = surface;
        return true;
    }
    bool RemoveSurface(size_t index)
    {
        if (!IsSurfaceExist(index)) return false;
        surfaces.erase(surfaces.begin() + index);
        return false;
    }

    inline bool IsRenderEnabled() { return enable_render; }
    inline void SetRenderEnabled(bool enable) { enable_render = enable; }
    inline void EnableRender() { enable_render = true; }
    inline void DisableRender() { enable_render = false; }

    void render(ShaderProgram *sp, glm::mat4 *view, glm::mat4 *projection)
    {
        if (!enable_render) return;

        glUseProgram(sp->GetShaderProgram());

        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(*view));

        /// TODO: добавить кеширование матрицы модели (maybe).
        glm::mat4 model = glm::mat4(1);

        model = glm::translate(model, pos);
        model = glm::rotate(model, rot.x, {1, 0, 0});
        model = glm::rotate(model, rot.y, {0, 1, 0});
        model = glm::rotate(model, rot.z, {0, 0, 1});
        model = glm::scale(model, scl);
        
        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniform4fv(glGetUniformLocation(sp->GetShaderProgram(), "color"), 1, glm::value_ptr(color));

        glUniform1i(glGetUniformLocation(sp->GetShaderProgram(), "texture"), 0);
        glActiveTexture(GL_TEXTURE0);

        for (Surface surface : surfaces)
        {
            Texture *texture = surface.GetTexture();
            Mesh *mesh = surface.GetMesh();
            FaceCullingType culling = surface.GetFaceCullingType();
            if (!(texture && mesh && mesh->HasBuffers() && texture->HasTexture() && culling != BothFaces)) continue;

            if (culling == NoCulling) glDisable(GL_CULL_FACE);
            else
            {
                glEnable(GL_CULL_FACE);

                switch (culling)
                {
                    case BackFace:
                        glCullFace(GL_BACK);
                        break;

                    case FrontFace:
                        glCullFace(GL_FRONT);
                        break;
                }
            }

            glBindTexture(GL_TEXTURE_2D, texture->GetTexture());

            glBindVertexArray(mesh->GetVAO());
            glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);
        }
    }
};

class Camera
{
  private:
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);
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

int main()
{
    GLFWwindow *window;
    int ret = initOpenGL(&window);
    if (ret != 0) return ret;
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram sp(vertexShaderSource, fragmentShaderSource);

    Camera cam = Camera();
    
    Mesh tri = Mesh();
    tri.LockBuffers();
    
    tri.AddVertexWithUV(-0.5, -0.5, 0.0, 0.0, 0.0);
    tri.AddVertexWithUV(-0.5, 0.5, 0.0, 0.0, 1.0);
    tri.AddVertexWithUV(0.5, -0.5, 0.0, 1.0, 0.0);
    tri.AddVertexWithUV(0.5, 0.5, 0.0, 1.0, 1.0);
    tri.AddTriangle(3, 1, 0);
    tri.AddTriangle(0, 2, 3);

    tri.GenerateBuffers();
    tri.UnlockBuffers();

    Texture tex = Texture();
    if (tex.LoadFromUCTEXFile("tex.uctex"))
    {
        std::cout << "Successfully loaded texture!" << std::endl;
        tex.SetDefaultParametres();
    }

    Texture tex16 = Texture();
    if (tex16.LoadFromUCTEXFile("tex16bit.uctex"))
    {
        std::cout << "Successfully loaded texture 2!" << std::endl;
        tex16.SetDefaultParametres();
    }

    Texture tex16_rgb = Texture();
    if (tex16_rgb.LoadFromUCTEXFile("tex16bit_rgb.uctex"))
    {
        std::cout << "Successfully loaded texture 3!" << std::endl;
        tex16_rgb.SetDefaultParametres();
    }

    Entity e = Entity();
    e.AddSurface(Surface(&tex, &tri, NoCulling));
    e.SetPosition({0.0, 0.0, -5.0});
    e.SetScale({1.0, 1.0, 1.0});
    e.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

    Entity e2 = Entity();
    e2.AddSurface(Surface(&tex16, &tri));
    e2.SetPosition({0.0, 0.0, -2.0});
    e2.SetScale({1.0, 1.0, 1.0});
    e2.SetColor({1.0f, 1.0f, 1.0f, 0.5f});

    Entity e3 = Entity();
    e3.AddSurface(Surface(&tex16_rgb, &tri));
    e3.SetPosition({0.0, 0.0, -10.0});
    e3.SetScale({1.0, 1.0, 1.0});

    float lastX = SCREEN_WIDTH / 2, lastY = SCREEN_HEIGHT / 2;
    glfwSetCursorPos(window, lastX, lastY);

    glfwSetTime(0);
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime() - prev_time;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        if (delta >= 1.0f / FPS)
        {
            prev_time = glfwGetTime();

            // ===== CONTROLS =====

            float speed;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = 6;
            else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) speed = 1;
            else speed = 3;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + cam.GetFront() * glm::vec3(speed * delta));
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - cam.GetFront() * glm::vec3(speed * delta));
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - cam.GetRight() * glm::vec3(speed * delta));
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + cam.GetRight() * glm::vec3(speed * delta));

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + glm::vec3(0, speed * delta, 0));
            if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - glm::vec3(0, speed * delta, 0));

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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            e.SetRotation(e.GetRotation() + glm::vec3(0, glm::radians(360.0f) * delta, 0));

            glm::mat4 view = cam.GetViewMatrix();
            glm::mat4 proj = cam.GetProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);
            e.render(&sp, &view, &proj);
            e2.render(&sp, &view, &proj);
            e3.render(&sp, &view, &proj);
            
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
        std::cerr << "Failed to initialize GLAD." << std::endl;
        glfwTerminate();
        return 1;
    }

    *window = w;

    return 0;
}