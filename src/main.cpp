#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <algorithm>
//#include <functional>
#include <cstdlib>

#include <csignal>

#include "opengl.hpp"
#include "glm.hpp"
#include "openal.hpp"

//#include <json-c/json.h>

//#include <AL/al.h>
//#include <AL/alc.h>

//#include "utils.hpp"
#include "objects.hpp"

const char *vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexturePosition;

out vec3 position;
out vec2 texturePosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    position = vertexPosition;
    texturePosition = vertexTexturePosition;

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
)";

const char *fragmentShaderSource = R"(
#version 330 core

in vec3 position;
in vec2 texturePosition;

out vec4 FragColor;

uniform vec4 color;

uniform bool hasTexture;
uniform sampler2D texture;

uniform bool fogEnabled;
uniform float fogStartDistance;
uniform float fogEndDistance;
uniform vec3 fogColor;

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;

uniform vec3 cameraFront;
uniform vec3 cameraUp;
uniform vec3 cameraRight;

void main()
{
    vec3 reltocam = position - cameraPosition;
    float distfromcam = length(reltocam);

    float fogIntensity = ((fogEndDistance - fogStartDistance) == 0) ? min(1.0, max(0.0, (distfromcam - fogStartDistance) / (fogEndDistance - fogStartDistance))) : 0.0;

    //FragColor = mix((hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color, fogColor, fogEnabled ? fogIntensity : 0.0);
    //FragColor = (hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color;

    //FragColor = mix((hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color, vec4(fogColor, 1.0), 0.0);
    FragColor = mix((hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color, vec4(fogColor, 1.0), fogEnabled ? fogIntensity : 0.0);
}
)";

int initOpenGL(GLFWwindow **window);

const unsigned int SCREEN_HEIGHT = 700;
const unsigned int FPS = 60;
const float MOUSE_SENSITIVITY = 0.1;
const float DEFAULT_CAMERA_SPEED = 3.0;

unsigned int windowWidth = 1200;
unsigned int windowHeight = 700;

float cameraSpeed = DEFAULT_CAMERA_SPEED;

void scrollCallback(GLFWwindow *w, double xoff, double yoff)
{
    cameraSpeed += yoff / 5;
    if (cameraSpeed < 0) cameraSpeed = 0;
}

void resizeCallback(GLFWwindow *w, int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, windowWidth, windowHeight);
}

void on_signal(int code)
{
    switch (code)
    {
        case SIGSEGV:
            std::cout << "!!! OCCURED SEGMENTATION FAULT !!!" << std::endl;
            break;
    }
}

int main()
{
    signal(SIGSEGV, on_signal);

    GLFWwindow *window;
    int ret = initOpenGL(&window);
    if (ret != 0) return ret;

    /*glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwSetWindowOpacity(window, 0.5);*/

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ALCdevice *aldev = alcOpenDevice(NULL);
    if (!aldev)
    {
        std::cout << "Failed to open OpenAL device." << std::endl;

        glfwTerminate();
        return 1;
    }

    ALCcontext *alctx = alcCreateContext(aldev, NULL);
    if (!alctx)
    {
        std::cout << "Failed to create OpenAL context." << std::endl;

        alcCloseDevice(aldev);
        glfwTerminate();
        return 1;
    }
    alcMakeContextCurrent(alctx);

    alDistanceModel(AL_LINEAR_DISTANCE);
    
    //ShaderProgram sp(vertexShaderSource, fragmentShaderSource);
    ShaderProgram sp;
    
    sp.LoadVertexShader(vertexShaderSource);
    sp.LoadFragmentShader(fragmentShaderSource);

    std::string log;
    if (!sp.CompileVertexShader(&log)) std::cout << "Compiling vertex shader error: \"" << log << "\"." << std::endl;
    if (!sp.CompileFragmentShader(&log)) std::cout << "Compiling fragment shader error: \"" << log << "\"." << std::endl;
    if (!sp.LinkShaderProgram(&log)) std::cout << "Linking shader program error: \"" << log << "\"." << std::endl;

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

    Mesh crowbar_head = Mesh();
    if (crowbar_head.LoadFromUCMESHFile("./models/crowbar/head.ucmesh"))
    {
        std::cout << "Successfully loaded model from \"./models/crowbar/head.ucmesh\" file." << std::endl;
    }

    Mesh crowbar_cyl = Mesh();
    if (crowbar_cyl.LoadFromUCMESHFile("./models/crowbar/cyl.ucmesh"))
    {
        std::cout << "Successfully loaded model from \"./models/crowbar/cyl.ucmesh\" file." << std::endl;
    }

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

    Texture crowbar_head_tex = Texture();
    if (crowbar_head_tex.LoadFromUCTEXFile("./textures/crowbar/head.uctex"))
    {
        std::cout << "Successfully loaded texture \"./textures/crowbar/head.uctex\"!" << std::endl;
        crowbar_head_tex.SetDefaultParametres();

        crowbar_head_tex.SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        crowbar_head_tex.SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Texture crowbar_cyl_tex = Texture();
    if (crowbar_cyl_tex.LoadFromUCTEXFile("./textures/crowbar/cyl.uctex"))
    {
        std::cout << "Successfully loaded texture \"./textures/crowbar/cyl.uctex\"!" << std::endl;
        crowbar_cyl_tex.SetDefaultParametres();

        crowbar_cyl_tex.SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        crowbar_cyl_tex.SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    /*
       ===== ===== =====
             MAIN
       ===== ===== =====
    */

    Entity e = Entity(Transform({0.0f, 0.0f, -5.0f}));
    e.surfaces.push_back(Surface(&tex, &tri, NoCulling));

    Entity e2 = Entity();
    e2.surfaces.push_back(Surface(&tex16, &tri));
    e2.color = {1.0f, 1.0f, 1.0f, 0.5f};

    Entity e3 = Entity(Transform({0.0f, 0.0f, -10.0f}, glm::quat(glm::vec3(0.0f)), {1.0f, 10.0f, 1.0f}));
    e3.surfaces.push_back(Surface(&tex16_rgb, &tri));

    Entity e4 = Entity(Transform({0, 0, -13}, glm::quat(glm::vec3(0)), {3.0f, 1.0f, 1.0f}));
    e4.surfaces.push_back(Surface(&tex_cube, &cube));

    Entity crowbar = Entity(Transform({-10, 0, 0}, glm::quat({0, 0, 0}), {0.1, 0.1, 0.1}));
    crowbar.surfaces.push_back(Surface(&crowbar_head, &crowbar_head_tex));
    crowbar.surfaces.push_back(Surface(&crowbar_cyl, &crowbar_cyl_tex));

    //Transform *tr_crowbar_cyl = &crowbar.surfaces[1].transform;

    Entity e_cube_surfrottest = Entity(Transform({5, 4, -4}));
    e_cube_surfrottest.surfaces.push_back(Surface(Transform({1, 1, 1}), &cube));
    e_cube_surfrottest.surfaces.push_back(Surface(&cube, &tex_cube));

    //Entity ground = Entity();
    //ground.surfaces.push_back(Surface(&tex_cube, &cube));
    //ground.SetScale({5.0f, 1.0f, 5.0f});

    //glm::vec3 scl = ground.GetScale();
    //AABB ground_coll = AABB({-0.5f * scl.x, -0.5f * scl.y, -0.5f * scl.z}, {0.5f * scl.x, 0.5f * scl.y, 0.5f * scl.z});

    //Entity prop = Entity();
    //prop.SetPosition({1.0f, 20.0f, 0.0f});
    //prop.surfaces.push_back(Surface(&tex_cube, &cube));

    //scl = prop.GetScale();
    //AABB prop_coll = AABB({-0.5f * scl.x, -0.5f * scl.y, -0.5f * scl.z}, {0.5f * scl.x, 0.5f * scl.y, 0.5f * scl.z});

    // ===== ===== MAIN ===== =====

    /*glm::vec3 vel = glm::vec3(0.0f);
    float mass = 1.0f;
    float inv_mass;
    if (mass > 0) inv_mass = 1.0f / mass;
    else inv_mass = 0;*/

    glm::vec3 v = glm::vec3(1.0f, 0.0f, 0.0f);
    std::cout << Utils::tostring(Utils::angles(v)) << std::endl;

    FogRenderSettings fogs;
    fogs.fogEnabled = true;
    fogs.fogColor = glm::vec3(1.0f, 1.0f, 1.0f);
    fogs.fogStartDistance = 0;
    fogs.fogEndDistance = 5;

    AudioClip testclip = AudioClip();
    if (testclip.LoadUCSOUNDFromFile("test.ucsound")) std::cout << "Successfully loaded sound from \"/test.ucsound\" file!" << std::endl;

    AudioSource source = AudioSource();
    source.SetParent(&e_cube_surfrottest, false);

    source.SetLooping(true);
    source.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
    source.SetSourceFloat(AL_MAX_DISTANCE, 5);

    source.PlayClip(&testclip);
    //source.transform = Transform();

    AudioClip zapclip = AudioClip();
    if (zapclip.LoadUCSOUNDFromFile("zapmachine.ucsound")) std::cout << "loaded sound \"/zapmachine.ucsound\"." << std::endl;

    AudioSource zapsrc = AudioSource();
    zapsrc.SetParent(&e, false);
    
    zapsrc.SetLooping(true);
    zapsrc.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
    zapsrc.SetSourceFloat(AL_MAX_DISTANCE, 5);

    zapsrc.PlayClip(&zapclip);

    //source.SetMaxDistance(2);
    //source.SetMinGain(0);
    //source.SetMaxGain(1);

    Entity relsys_e_parent = Entity(Transform({-3, 0, 5}));
    relsys_e_parent.surfaces.push_back(Surface(&cube));
    relsys_e_parent.color = {0, 1, 0, 1};

    Entity relsys_e_child = Entity(Transform());
    relsys_e_child.SetParent(&relsys_e_parent, false);
    relsys_e_child.transform.SetPosition({0, 0, 2});
    relsys_e_child.surfaces.push_back(Surface(&cube));
    relsys_e_child.color = {1, 0, 1, 1};

    AudioListener listener = AudioListener();
    listener.SetParent(&cam, false);

    bool lmb_pressed = false;
    float lastX = windowWidth / 2, lastY = windowHeight / 2;
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

            /*if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);*/

            /*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !lmb_pressed)
            {
                lmb_pressed = true;
                std::cout << "clicked" << std::endl;

                glfwSetCursorPos(window, lastX, lastY);

                if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && lmb_pressed) lmb_pressed = false;*/

            // ===== CONTROLS =====
            if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL)
            {
                float speed;
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = cameraSpeed * 2.0;
                else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) speed = cameraSpeed / 2.0;
                else speed = cameraSpeed;

                Transform *t = &cam.transform;

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) t->Translate(t->GetFront() * glm::vec3(speed * delta));
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) t->Translate(-t->GetFront() * glm::vec3(speed * delta));
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) t->Translate(-t->GetRight() * glm::vec3(speed * delta));
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) t->Translate(t->GetRight() * glm::vec3(speed * delta));

                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) t->Translate(glm::vec3(0, speed * delta, 0));
                if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) t->Translate(glm::vec3(0, -speed * delta, 0));

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) cameraSpeed = DEFAULT_CAMERA_SPEED;

                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                //std::cout << glm::degrees(r.x) << " " << glm::degrees(r.y) << " " << glm::degrees(r.z) << std::endl;

                cam.transform.Rotate(glm::quat(glm::vec3(0, -glm::radians((mouseX - lastX) * MOUSE_SENSITIVITY), 0)));
                cam.transform.Rotate(glm::quat(glm::vec3(-glm::radians((mouseY - lastY) * MOUSE_SENSITIVITY), 0, 0)));

                glm::vec3 r = glm::eulerAngles(t->GetRotation());
                if (glm::degrees(r.x) > 89.0f) cam.transform.SetRotation(glm::quat({89.0f, r.y, r.z}));
                if (glm::degrees(r.x) < -89.0f) cam.transform.SetRotation(glm::quat({-89.0f, r.y, r.z}));
                //t->SetRotation(r);

                r = glm::eulerAngles(t->GetRotation());
                cam.transform.SetRotation(glm::quat({r.x, r.y, 0}));

                lastX = mouseX;
                lastY = mouseY;
            }

            // ===== MAIN =====
            
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            e.transform.Rotate(glm::vec3(0, glm::radians(360.0f) * delta, 0));

            //tr_crowbar_cyl->Rotate(glm::vec3(glm::radians(360.0f) * delta, 0, 0));

            e_cube_surfrottest.surfaces[0].transform.Rotate(glm::vec3(glm::radians(360.0f) * delta, 0, 0));
            e_cube_surfrottest.transform.Rotate(glm::vec3(0, glm::radians(90.0f) * delta, 0));
            e_cube_surfrottest.transform.Translate({0, 0, -1 * delta});

            //source.transform.SetPosition(e_cube_surfrottest.transform.GetPosition());

            //std::cout << source.GetGlobalTransform().ToString() << std::endl;

            //e4.SetRotation(e4.GetRotation() + glm::vec3(0, glm::radians(90.0f) * delta, glm::radians(30.0f) * delta));

            relsys_e_parent.transform.Rotate(glm::quat({0, glm::radians(45.0f) * delta, 0}));

            /*prop.SetPosition(prop.GetPosition() + glm::vec3(0.0f, -5.0f * delta, 0.0f));

            AABB currAABB = prop_coll.Copy();
            currAABB.Translate(prop.GetPosition());

            std::cout << ground_coll.GetAABBPenetration(&currAABB).y << std::endl;
            if (currAABB.AABBIntersects(ground_coll)) std::cout << "Intersects!" << std::endl;

            prop.SetPosition(prop.GetPosition() + ground_coll.GetAABBPushForce(&currAABB));*/

            /*AABB currAABB = prop_coll.Copy();
            currAABB.Translate(prop.GetPosition());

            glm::vec3 pen = ground_coll.GetAABBWeightedPenetration(&currAABB);
            glm::vec3 pennorm = Utils::normalize(pen);

            glm::vec3 force = glm::vec3(0.0f, -9.8f * mass, 0.0f);

            force -= pennorm * force;

            glm::vec3 accel = force / glm::vec3(mass);

            vel += accel * glm::vec3(delta);
            vel -= pennorm * vel;

            prop.SetPosition(prop.GetPosition() + vel * glm::vec3(delta) + pen - glm::vec3(0.00001f) * pennorm);

            std::cout << "pennorm Y: " << pennorm.y << ", force Y: " << force.y << ", vel Y: " << vel.y << ", pos Y: " << prop.GetPosition().y << std::endl;*/

            //AABB currAABB = prop_coll.Copy();
            //currAABB.Translate(prop.GetPosition());

            /*glm::vec3 pen = ground_coll.GetAABBPenetration(&currAABB);
            glm::vec3 pushDir = Utils::normalize(currAABB.GetCenterOffset() - ground_coll.GetAABBOverlapRegion(&currAABB).GetCenterOffset());
            
            glm::vec3 force = glm::vec3(0.0f, -9.8f * mass, 0.0f);

            force -= pushDir * pen;

            glm::vec3 accel = force / glm::vec3(mass);

            vel += accel * glm::vec3(delta);
            //vel -= vel * pushDir * pen;

            prop.SetPosition(prop.GetPosition() + vel * glm::vec3(delta) + pen);*/

            /*if (ground_coll.AABBIntersects(&currAABB))
            {
                glm::vec3 relvel = vel;
                
            }*/

            /*alListenerfv(AL_POSITION, glm::value_ptr(cam.transform.GetPosition()));
            
            ListenerOrientation orient;
            orient.front = cam.transform.GetFront();
            orient.up = cam.transform.GetUp();
            alListenerfv(AL_ORIENTATION, (ALfloat *)&orient);*/

            glm::mat4 view = cam.GetViewMatrix();
            glm::mat4 proj = cam.GetProjectionMatrix(windowWidth, windowHeight);

            e.Render(&sp, &view, &proj, &cam.transform, &fogs);
            e3.Render(&sp, &view, &proj, &cam.transform, &fogs);
            e4.Render(&sp, &view, &proj, &cam.transform, &fogs);

            crowbar.Render(&sp, &view, &proj, &cam.transform, &fogs);

            e_cube_surfrottest.Render(&sp, &view, &proj, &cam.transform, &fogs);

            e2.Render(&sp, &view, &proj, &cam.transform, &fogs);

            relsys_e_parent.Render(&sp, &view, &proj, &cam.transform, &fogs);
            relsys_e_child.Render(&sp, &view, &proj, &cam.transform, &fogs);

            //ground.Render(&sp, &view, &proj);
            //prop.Render(&sp, &view, &proj);
            
            glfwSwapBuffers(window);

        }
        glfwPollEvents();
    }

    std::cout << "successful quit" << std::endl;
    
    alcCloseDevice(aldev);
    glfwTerminate();
    return 0;
}

int initOpenGL(GLFWwindow **window)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //GLFWwindow *w = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
    GLFWmonitor *m = glfwGetPrimaryMonitor();
    if (!m)
    {
        std::cout << "Can't find primary monitor. Program halted with error exit code." << std::endl;
        glfwTerminate();
        return 1;
    }

    int width, height;
    glfwGetMonitorWorkarea(m, NULL, NULL, &width, &height);
    /*windowWidth = width;
    windowHeight = height;*/

    GLFWwindow *w = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL/*m*/, NULL);
    if (!window)
    {
        std::cout << "Can't initialize GLFW window. Program halted with error exit code." << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD. Program halted with error exit code." << std::endl;
        glfwTerminate();
        return 1;
    }

    *window = w;
    
    glViewport(0, 0, windowWidth, windowHeight);

    return 0;
}