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
#include <functional>
#include <cstdlib>

#include <csignal>

#include <Jolt/Jolt.h>

#include "opengl.hpp"
#include "glm.hpp"
#include "openal.hpp"

#include "objects.hpp"

const char *vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexturePosition;

out vec3 globalVertexPosition;
out vec2 texturePosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 globvpos4 = model * vec4(vertexPosition, 1.0);

    globalVertexPosition = vec3(globvpos4.x, globvpos4.y, globvpos4.z);
    texturePosition = vertexTexturePosition;

    //gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    gl_Position = projection * view * globvpos4;
}
)";

const char *fragmentShaderSource = R"(
#version 330 core

in vec3 globalVertexPosition;
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
    vec4 vertcol = (hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color;

    float dist = length(globalVertexPosition - cameraPosition);
    float fog_int_factor = min(1, max(0, (dist - fogStartDistance) / (fogEndDistance - fogStartDistance)));

    FragColor = mix(vertcol, vec4(fogColor, 1), fog_int_factor);
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

GLFWwindow *window;

#include "testentities.hpp"

int main()
{
    signal(SIGSEGV, on_signal);

    int ret = initOpenGL(&window);
    if (ret != 0) return ret;
    int exitcode = 0;

    /*glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwSetWindowOpacity(window, 0.5);*/

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    AudioDevice *dev = nullptr;
    try
    {
        dev = new AudioDevice(NULL);
        AudioSystem::SetCurrentDevice(dev);
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        exitcode = 1;
        goto quit;
    }

    AudioSystem::SetDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);


    // ========================================
    // ========================================
    // ========================================
    
    {
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
        
        tri.AddVertexWithUV(-0.5, -0.5, 0.0, 0.0, 0.0);
        tri.AddVertexWithUV(-0.5, 0.5, 0.0, 0.0, 1.0);
        tri.AddVertexWithUV(0.5, -0.5, 0.0, 1.0, 0.0);
        tri.AddVertexWithUV(0.5, 0.5, 0.0, 1.0, 1.0);
        tri.AddTriangle(3, 1, 0);
        tri.AddTriangle(0, 2, 3);

        tri.GenerateBuffers();

        Mesh cube = Mesh();

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

        Entity e = Entity(Transform({0.0f, 0.0f, -7.5f}));
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


        Entity e_cube_surfrottest = Entity(Transform({5, 4, -4}));
        e_cube_surfrottest.surfaces.push_back(Surface(Transform({1, 1, 1}), &cube));
        e_cube_surfrottest.surfaces[0].color = {0, 0, 0, 1};
        e_cube_surfrottest.surfaces.push_back(Surface(&cube, &tex_cube));


        AudioEffectSlot reverbslot = AudioEffectSlot();
        {
            AudioEffectProperties eff = AudioEffectProperties();
            eff.SetEffectType(AL_EFFECT_EAXREVERB);

            eff.SetEffectFloat(AL_EAXREVERB_DENSITY, 1);
            eff.SetEffectFloat(AL_EAXREVERB_DIFFUSION, 0.9);
            eff.SetEffectFloat(AL_EAXREVERB_GAIN, 0.3);
            //eff.SetEffectFloat(AL_EAXREVERB_DECAY_TIME, 3.2);
            eff.SetEffectFloat(AL_EAXREVERB_DECAY_TIME, 5);
            eff.SetEffectFloat(AL_EAXREVERB_DECAY_HFRATIO, 0.7);
            eff.SetEffectFloat(AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, 0.1);

            //eff.SetEffectFloat(AL_EAXREVERB_ECHO_DEPTH, 1);

            reverbslot.ApplyEffect(eff);
        }

        glm::vec3 v = glm::vec3(1.0f, 0.0f, 0.0f);
        std::cout << Utils::tostring(Utils::angles(v)) << std::endl;

        AudioClip testclip = AudioClip();
        if (testclip.LoadFromUCSOUNDFile("test.ucsound")) std::cout << "Successfully loaded sound from \"/test.ucsound\" file!" << std::endl;

        AudioSource source = AudioSource();
        source.SetParent(&e_cube_surfrottest, false);

        source.SetLooping(true);
        source.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
        source.SetSourceFloat(AL_MAX_DISTANCE, 5);

        source.SetCurrentClip(&testclip);
        source.Play();


        Texture maxwellcat_tex = Texture();
        if (maxwellcat_tex.LoadFromUCTEXFile("textures/maxwell.uctex"))
        {
            printf("loaded \"textures/maxwell.uctex\"\n");

            maxwellcat_tex.SetDefaultParametres();
            maxwellcat_tex.SetLinearSmoothing();
        }

        Mesh maxwellcat_mesh = Mesh();
        if (maxwellcat_mesh.LoadFromUCMESHFile("models/maxwell_the_cat.ucmesh")) printf("loaded \"models/maxwell_the_cat.ucmesh\"\n");

        glm::vec3 maxwellcat_default_scale = glm::vec3(0.05);//glm::vec3(0.0005);
        Entity maxwellcat = Entity(Transform({0, 0, -5}, glm::quat(glm::vec3(0)), maxwellcat_default_scale));
        maxwellcat.surfaces.push_back(Surface(&maxwellcat_mesh, &maxwellcat_tex));


        AudioClip zapclip = AudioClip();
        if (zapclip.LoadFromUCSOUNDFile("zapmachine.ucsound")) std::cout << "loaded sound \"/zapmachine.ucsound\"." << std::endl;

        AudioSource zapsrc = AudioSource();
        zapsrc.SetParent(&maxwellcat, false);
        reverbslot.AddSource(&zapsrc);
        
        zapsrc.SetLooping(true);
        zapsrc.SetSourceFloat(AL_REFERENCE_DISTANCE, 4);
        zapsrc.SetSourceFloat(AL_MAX_DISTANCE, 16);
        zapsrc.SetSourceFloat(AL_GAIN, 1.0f);

        zapsrc.SetCurrentClip(&zapclip);
        zapsrc.Pause();


        AudioClip labdroneclip = AudioClip();
        if (labdroneclip.LoadFromUCSOUNDFile("labdrone2.ucsound")) std::cout << "loaded sound \"/labdrone2.ucsound\"." << std::endl;

        AudioSource labdronesrc = AudioSource();
        labdronesrc.SetParent(&zapsrc, false);
        reverbslot.AddSource(&labdronesrc);
        
        labdronesrc.SetLooping(true);
        labdronesrc.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
        labdronesrc.SetSourceFloat(AL_MAX_DISTANCE, 16);
        labdronesrc.SetSourceFloat(AL_GAIN, 1.0f);

        labdronesrc.SetCurrentClip(&labdroneclip);



        AudioSource labdronesrcpitch150 = AudioSource();
        labdronesrcpitch150.SetParent(&labdronesrc, false);
        reverbslot.AddSource(&labdronesrcpitch150);
        
        labdronesrcpitch150.SetLooping(true);
        labdronesrcpitch150.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
        labdronesrcpitch150.SetSourceFloat(AL_MAX_DISTANCE, 16);
        labdronesrcpitch150.SetSourceFloat(AL_PITCH, 1.5f);
        labdronesrcpitch150.SetSourceFloat(AL_GAIN, 1.0f);

        labdronesrcpitch150.SetCurrentClip(&labdroneclip);


        AudioClip steamburstclip = AudioClip();
        steamburstclip.LoadFromUCSOUNDFile("sfx/steamburst.ucsound");

        AudioClip lightswitch2clip = AudioClip();
        if (lightswitch2clip.LoadFromUCSOUNDFile("sfx/button/lightswitch2.ucsound")) std::cout << "loaded \"sfx/button/lightswitch2.ucsound\"" << std::endl;

        AudioSource ambsrc = AudioSource();
        ambsrc.SetParent(&zapsrc, false);
        reverbslot.AddSource(&ambsrc);

        ambsrc.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
        ambsrc.SetSourceFloat(AL_MAX_DISTANCE, 16);
        ambsrc.SetSourceFloat(AL_GAIN, 0.3f);


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


        AudioClip button8sfx = AudioClip();
        if (button8sfx.LoadFromUCSOUNDFile("sfx/button/8.ucsound")) std::cout << "loaded \"sfx/button/8.ucsound\"" << std::endl;
        AudioClip button10sfx = AudioClip();
        if (button10sfx.LoadFromUCSOUNDFile("sfx/button/10.ucsound")) std::cout << "loaded \"sfx/button/10.ucsound\"" << std::endl;

        AudioClip button3sfx = AudioClip();
        if (button3sfx.LoadFromUCSOUNDFile("sfx/button/3.ucsound")) std::cout << "loaded \"sfx/button/3.ucsound\"" << std::endl;
        AudioClip button2sfx = AudioClip();
        if (button2sfx.LoadFromUCSOUNDFile("sfx/button/2.ucsound")) std::cout << "loaded \"sfx/button/2.ucsound\"" << std::endl;


        Texture btn4_off = Texture();
        if (btn4_off.LoadFromUCTEXFile("textures/button/4_off.uctex"))
        {
            std::cout << "loaded \"textures/button/4_off.uctex\"" << std::endl;
            btn4_off.SetDefaultParametres();
            btn4_off.SetLinearSmoothing();
        }

        Texture btn4_on = Texture();
        if (btn4_on.LoadFromUCTEXFile("textures/button/4_on.uctex"))
        {
            std::cout << "loaded \"textures/button/4_on.uctex\"" << std::endl;
            btn4_on.SetDefaultParametres();
            btn4_on.SetLinearSmoothing();
        }

        Texture btn3_off = Texture();
        if (btn3_off.LoadFromUCTEXFile("textures/button/3_off.uctex"))
        {
            std::cout << "loaded \"textures/button/3_off.uctex\"" << std::endl;
            btn3_off.SetDefaultParametres();
            btn3_off.SetLinearSmoothing();
        }

        Texture btn3_on = Texture();
        if (btn3_on.LoadFromUCTEXFile("textures/button/3_on.uctex"))
        {
            std::cout << "loaded \"textures/button/3_on.uctex\"" << std::endl;
            btn3_on.SetDefaultParametres();
            btn3_on.SetLinearSmoothing();
        }


        Mesh rect_button = Mesh();
        if (rect_button.LoadFromUCMESHFile("models/buttons/4.ucmesh")) std::cout << "loaded \"/models/buttons/4.ucmesh\"" << std::endl;

        Mesh square_button = Mesh();
        if (square_button.LoadFromUCMESHFile("models/buttons/3.ucmesh")) std::cout << "loaded \"/models/buttons/3.ucmesh\"" << std::endl;

        HL1ToggleButtonSettings setts;

        setts.mesh = &rect_button;
        setts.interaction_sfx = &button8sfx;
        setts.locked_sfx = &button10sfx;
        setts.on_texture = &btn4_on;
        setts.off_texture = &btn4_off;
        HL1ToggleButton btn = HL1ToggleButton(Transform({0, 0, -2}, glm::quat(glm::radians(glm::vec3(0, 180, 0)))), setts);

        setts.mesh = &square_button;
        setts.interaction_sfx = &button3sfx;
        setts.locked_sfx = &button2sfx;
        setts.on_texture = &btn3_on;
        setts.off_texture = &btn3_off;
        HL1ToggleButton btn2 = HL1ToggleButton(Transform({1.5, 0, -2}, glm::quat(glm::radians(glm::vec3(0, 180, 0)))), setts);

        btn2.SetInteractionCooldown(0.5);

        float ams_run_progress = 0;
        float ams_run_step_per_second = 0;
        const float AMS_RUN_ANIM_TIME = 10;

        btn.OnClickCallback = [&](HL1ToggleButton *button)
        {
            if (button->IsEnabled()) ams_run_step_per_second = 1 / AMS_RUN_ANIM_TIME;
            else ams_run_step_per_second = -1 / AMS_RUN_ANIM_TIME;

            zapsrc.Play();

            ambsrc.SetCurrentClip(&lightswitch2clip);
            ambsrc.Play();
        };

        btn2.OnClickCallback = [&](HL1ToggleButton *button)
        {
            if (button->IsEnabled())
            {
                labdronesrc.Play();
                labdronesrcpitch150.Play();
            }
            else
            {
                labdronesrc.Rewind();
                labdronesrcpitch150.Rewind();
                
                maxwellcat.transform.SetScale(maxwellcat_default_scale);
            }
        };

        FogRenderSettings fogs;
        fogs.fogEnabled = true;
        fogs.fogColor = glm::vec3(1.0f, 1.0f, 1.0f);
        fogs.fogStartDistance = 2;
        fogs.fogEndDistance = 20;

        bool f_pressed = false;

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

                    float mxoff = -glm::radians((mouseX - lastX) * MOUSE_SENSITIVITY);
                    float myoff = -glm::radians((mouseY - lastY) * MOUSE_SENSITIVITY);
                    {
                        glm::quat delta_pitch = glm::angleAxis(myoff, glm::vec3(1, 0, 0));
                        glm::quat delta_yaw = glm::angleAxis(mxoff, glm::vec3(0, 1, 0));

                        glm::quat new_rotation = delta_yaw * t->GetRotation() * delta_pitch;

                        glm::vec3 front = new_rotation * glm::vec3(0, 0, -1);
                        glm::vec3 front_xz = glm::normalize(glm::vec3(front.x, 0, front.z));

                        if (glm::dot(front, front_xz) >= glm::cos(glm::radians(60.0f))) t->SetRotation(new_rotation);
                        else t->Rotate(delta_yaw);
                    }

                    lastX = mouseX;
                    lastY = mouseY;

                    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !f_pressed)
                    {
                        f_pressed = true;

                        AudioSourceState st = zapsrc.GetState();
                        if (st == PAUSED)
                        {
                            zapsrc.Play();
                            printf("Resumed zapsrc.\n");
                        }
                        else if (st == PLAYING)
                        {
                            zapsrc.Pause();
                            printf("Paused zapsrc.\n");
                        }
                    }
                    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && f_pressed) f_pressed = false;
                }

                // ===== MAIN =====

                btn.Update(delta, &cam.transform);
                btn2.Update(delta, &cam.transform);

                if (ams_run_step_per_second > 0)
                {
                    if (ams_run_progress < 1) ams_run_progress += ams_run_step_per_second * delta;
                    else
                    {
                        ams_run_progress = 1;
                        ams_run_step_per_second = 0;
                    }

                    if ((ams_run_progress >= 4 / AMS_RUN_ANIM_TIME && ams_run_progress < (4 + ams_run_step_per_second) / AMS_RUN_ANIM_TIME) ||\
                        (ams_run_progress >= 8 / AMS_RUN_ANIM_TIME && ams_run_progress < (8 + ams_run_step_per_second) / AMS_RUN_ANIM_TIME))
                    {
                        ambsrc.SetCurrentClip(&steamburstclip);
                        ambsrc.Play();
                    }
                }
                else if (ams_run_step_per_second < 0)
                {
                    if (ams_run_progress > 0) ams_run_progress += ams_run_step_per_second * delta;
                    else
                    {
                        ams_run_progress = 0;
                        ams_run_step_per_second = 0;
                    }
                }

                if (ams_run_progress == 0) zapsrc.Pause();
                else zapsrc.SetSourceFloat(AL_PITCH, ams_run_progress);

                btn2.locked = ams_run_progress != 1;
                if (btn2.IsEnabled() && ams_run_progress < 1) btn2.SetEnabled(false);
                btn.locked = (ams_run_progress != 0 && ams_run_progress != 1) || btn2.IsEnabled();
                
                //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClearColor(fogs.fogColor.x, fogs.fogColor.y, fogs.fogColor.z, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                maxwellcat.transform.Rotate(glm::vec3(0, ams_run_progress * glm::radians(360.0f * 4 * 4) * delta, glm::radians(45.0f) * delta * (btn2.IsEnabled() ? 1 : 0)));
                if (btn2.IsEnabled()) maxwellcat.transform.SetScale(maxwellcat_default_scale + maxwellcat_default_scale * glm::vec3(0, glm::sin(glm::radians(90 * glfwGetTime())), 0));


                e_cube_surfrottest.surfaces[0].transform.Rotate(glm::vec3(glm::radians(360.0f) * delta, 0, 0));
                e_cube_surfrottest.transform.Rotate(glm::vec3(0, glm::radians(90.0f) * delta, 0));
                e_cube_surfrottest.transform.Translate({0, 0, -1 * delta});

                
                relsys_e_parent.transform.Rotate(glm::quat({0, glm::radians(45.0f) * delta, 0}));


                glm::mat4 view = cam.GetViewMatrix();
                glm::mat4 proj = cam.GetProjectionMatrix(windowWidth, windowHeight);
                Transform camt = cam.GetGlobalTransform();

                e.Render(&sp, &view, &proj, &camt, &fogs);
                e3.Render(&sp, &view, &proj, &camt, &fogs);
                e4.Render(&sp, &view, &proj, &camt, &fogs);

                crowbar.Render(&sp, &view, &proj, &camt, &fogs);

                e_cube_surfrottest.Render(&sp, &view, &proj, &camt, &fogs);

                relsys_e_parent.Render(&sp, &view, &proj, &camt, &fogs);
                relsys_e_child.Render(&sp, &view, &proj, &camt, &fogs);

                btn.Render(&sp, &view, &proj, &camt, &fogs);
                btn2.Render(&sp, &view, &proj, &camt, &fogs);

                maxwellcat.Render(&sp, &view, &proj, &camt, &fogs);

                e2.Render(&sp, &view, &proj, &camt, &fogs);
                
                glfwSwapBuffers(window);

            }
            glfwPollEvents();
        }
    }

    quit:
    
    if (dev) delete dev;
    glfwTerminate();

    if (exitcode == EXIT_SUCCESS) std::cout << "successful quit" << std::endl;

    return exitcode;
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
    
    GLFWmonitor *m = glfwGetPrimaryMonitor();
    if (!m)
    {
        std::cout << "Can't find primary monitor. Program halted with error exit code." << std::endl;
        glfwTerminate();
        return 1;
    }

    int width, height;
    glfwGetMonitorWorkarea(m, NULL, NULL, &width, &height);

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