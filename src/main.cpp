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

#include "objects.hpp"

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

uniform bool hasTexture;
uniform sampler2D texture;

void main()
{
    if (hasTexture) FragColor = texture2D(texture, TexCoord) * color;
    else FragColor = color;
}
)";

int initOpenGL(GLFWwindow **window);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 700;
const unsigned int FPS = 60;
const float MOUSE_SENSITIVITY = 0.1;
const float DEFAULT_CAMERA_SPEED = 3.0;

float cameraSpeed = DEFAULT_CAMERA_SPEED;

void scrollCallback(GLFWwindow *w, double xoff, double yoff)
{
    cameraSpeed += yoff / 5;
    if (cameraSpeed < 0) cameraSpeed = 0;
}

int main()
{
    GLFWwindow *window;
    int ret = initOpenGL(&window);
    if (ret != 0) return ret;
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scrollCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram sp(vertexShaderSource, fragmentShaderSource);

    Camera cam = Camera();

    // ===== MESHES =====
    
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

    Mesh cube = Mesh();
    cube.LockBuffers();

    cube.AddVertexWithUV(-0.5, -0.5, -0.5, 0 / 4.0f, 1 / 2.0f); // 0
    cube.AddVertexWithUV(0.5, -0.5, -0.5, 1 / 4.0f, 1 / 2.0f); // 1
    cube.AddVertexWithUV(-0.5, 0.5, -0.5, 0.0f, 0.0f); // 2
    cube.AddVertexWithUV(-0.5, -0.5, 0.5, 3 / 4.0f, 1 / 2.0f); // 3

    cube.AddVertexWithUV(-0.5, 0.5, 0.5, 3 / 4.0f, 0.0f); // 4
    cube.AddVertexWithUV(0.5, -0.5, 0.5, 2 / 4.0f, 1 / 2.0f); // 5
    cube.AddVertexWithUV(0.5, 0.5, -0.5, 1 / 4.0f, 0.0f); // 6
    cube.AddVertexWithUV(0.5, 0.5, 0.5, 2 / 4.0f, 0.0f); // 7

    cube.AddVertexWithUV(-0.5, -0.5, -0.5, 4 / 4.0f, 1 / 2.0f); // 8 (at 0)
    cube.AddVertexWithUV(-0.5, 0.5, -0.5, 4 / 4.0f, 0.0f); // 9 (at 2)

    cube.AddVertexWithUV(-0.5, -0.5, 0.5, 0.0f, 2 / 2.0f); // 10 (at 3)
    cube.AddVertexWithUV(0.5, -0.5, 0.5, 1 / 4.0f, 2 / 2.0f); // 11 (at 5)
    
    cube.AddVertexWithUV(-0.5, 0.5, 0.5, 1 / 4.0f, 1 / 2.0f); // 12 (at 4)
    cube.AddVertexWithUV(0.5, 0.5, 0.5, 2 / 4.0f, 1 / 2.0f); // 13 (at 7)
    cube.AddVertexWithUV(-0.5, 0.5, -0.5, 1 / 4.0f, 2 / 2.0f); // 14 (at 9)
    cube.AddVertexWithUV(0.5, 0.5, -0.5, 2 / 4.0f, 2 / 2.0f); // 15 (at 6)

    // Front face.
    cube.AddTriangle(0, 2, 6);
    cube.AddTriangle(6, 1, 0);

    // Left face.
    cube.AddTriangle(4, 9, 8);
    cube.AddTriangle(8, 3, 4);

    // Right face.
    cube.AddTriangle(7, 5, 1);
    cube.AddTriangle(1, 6, 7);

    // Back face.
    cube.AddTriangle(7, 4, 3);
    cube.AddTriangle(3, 5, 7);

    // Down face.
    cube.AddTriangle(0, 1, 11);
    cube.AddTriangle(11, 10, 0);

    cube.AddTriangle(14, 12, 13);
    cube.AddTriangle(13, 15, 14);

    cube.GenerateBuffers();
    cube.UnlockBuffers();

    // ===== TEXTURES =====

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

    Texture tex_cube = Texture();
    if (tex_cube.LoadFromUCTEXFile("cube.uctex"))
    {
        std::cout << "Successfully loaded texture \"cube.uctex\"!" << std::endl;
        tex_cube.SetDefaultParametres();
    }

    /*
       ===== ===== =====
             MAIN
       ===== ===== =====
    */

    Entity e = Entity();
    e.AddSurface(Surface(&tex, &tri, NoCulling));
    e.SetPosition({0.0, 0.0, -5.0});
    e.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

    Entity e2 = Entity();
    e2.AddSurface(Surface(&tex16, &tri));
    e2.SetPosition({0.0, 0.0, -2.0});
    e2.SetColor({1.0f, 1.0f, 1.0f, 0.5f});

    Entity e3 = Entity();
    e3.AddSurface(Surface(&tex16_rgb, &tri));
    e3.SetPosition({0.0, 0.0, -10.0});
    e3.SetScale({1.0f, 10.0f, 1.0f});

    Entity e4 = Entity();
    e4.AddSurface(Surface(&tex_cube, &cube));
    e4.SetPosition({0, 0, -13});
    e4.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
    e4.SetScale({3.0f, 1.0f, 1.0f});

    // ===== ===== MAIN ===== =====

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
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = cameraSpeed * 2.0;
            else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) speed = cameraSpeed / 2.0;
            else speed = cameraSpeed;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + cam.GetFront() * glm::vec3(speed * delta));
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - cam.GetFront() * glm::vec3(speed * delta));
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - cam.GetRight() * glm::vec3(speed * delta));
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + cam.GetRight() * glm::vec3(speed * delta));

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() + glm::vec3(0, speed * delta, 0));
            if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) cam.SetPosition(cam.GetPosition() - glm::vec3(0, speed * delta, 0));

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) cameraSpeed = DEFAULT_CAMERA_SPEED;

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

            //e4.SetRotation(e4.GetRotation() + glm::vec3(0, glm::radians(90.0f) * delta, glm::radians(30.0f) * delta));

            glm::mat4 view = cam.GetViewMatrix();
            glm::mat4 proj = cam.GetProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);
            e.Render(&sp, &view, &proj);
            e2.Render(&sp, &view, &proj);
            e3.Render(&sp, &view, &proj);
            e4.Render(&sp, &view, &proj);
            
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