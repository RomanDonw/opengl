#include "Camera.hpp"

Camera::Camera(Transform t, float _fov, float _neardist, float _fardist) : GameObject(t), fov(_fov), neardist(_neardist), fardist(_fardist) {}
Camera::Camera(float _fov, float _neardist, float _fardist) : GameObject(), fov(_fov), neardist(_neardist), fardist(_fardist) {}

Camera::Camera(Transform t, float _neardist, float _fardist) : GameObject(t), neardist(_neardist), fardist(_fardist) {}
Camera::Camera(float _neardist, float _fardist) : GameObject(), neardist(_neardist), fardist(_fardist) {}

Camera::Camera(Transform t, float _fov) : GameObject(t), fov(_fov) {}
Camera::Camera(float _fov) : GameObject(), fov(_fov) {}

Camera::Camera(Transform t) : GameObject(t) {}
Camera::Camera() : GameObject() {}

Camera::~Camera() {}

float Camera::GetNearDistance() { return neardist; }
float Camera::GetFarDistance() { return fardist; }
float Camera::GetFOV() { return fov; }

void Camera::SetNearDistance(float _neardist) { neardist = _neardist; }
void Camera::SetFarDistance(float _fardist) { fardist = _fardist; }
void Camera::SetFOV(float _fov) { fov = _fov; }

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetFront(), transform.GetUp());
}

glm::mat4 Camera::GetProjectionMatrix(unsigned int screen_width, unsigned int screen_height)
{
    return glm::perspective(fov, (float)screen_width / (float)screen_height, neardist, fardist);
}