#include "AudioListener.hpp"

#include "../../openal.hpp"

#include <exception>
#include <stdexcept>

// === PRIVATE ===

bool AudioListener::hasListener = false;

struct
{
    glm::vec3 front;
    glm::vec3 up;
} typedef ALListenerOrientation;

void AudioListener::constructor()
{
    if (hasListener) throw std::runtime_error("can be exist only one OpenAL listener");
    hasListener = true;
}

void AudioListener::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    Transform globt = GetGlobalTransform();

    alListenerfv(AL_POSITION, glm::value_ptr(globt.GetPosition()));

    ALListenerOrientation orient;
    orient.front = globt.GetFront();
    orient.up = globt.GetUp();
    alListenerfv(AL_ORIENTATION, (ALfloat *)&orient);
}

// === PUBLIC ===

AudioListener::AudioListener(Transform t) : GameObject(t) { constructor(); };
AudioListener::AudioListener() : GameObject() { constructor(); };

AudioListener::~AudioListener() { hasListener = false; }