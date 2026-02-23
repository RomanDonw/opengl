#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../glm.hpp"

#include "Transform.hpp"
#include "GameObject.hpp"

class Camera : public GameObject
{
  private:
    float neardist = 0.05;//0.1;
    float fardist = 1000;
    float fov = glm::radians(60.0f);
    
  public:
    const GameObjectType type = CAMERA;

    Camera(Transform t, float _fov, float _neardist, float _fardist);
    Camera(float _fov, float _neardist, float _fardist);

    Camera(Transform t, float _neardist, float _fardist);
    Camera(float _neardist, float _fardist);

    Camera(Transform t, float _fov);
    Camera(float _fov);

    Camera(Transform t);
    Camera();

    ~Camera();

    float GetNearDistance();
    float GetFarDistance();
    float GetFOV();

    void SetNearDistance(float _neardist);
    void SetFarDistance(float _fardist);
    void SetFOV(float _fov);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(unsigned int screen_width, unsigned int screen_height);
};

#endif