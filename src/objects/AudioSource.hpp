#ifndef AUDIOSOURCE_HPP
#define AUDIOSOURCE_HPP

#include "../openal.hpp"

#include "Transform.hpp"
#include "GameObject.hpp"
#include "AudioSourceTransform.hpp"
#include "AudioClip.hpp"

class AudioSource : public GameObject
{
  private:
    ALuint source;
    bool looped = false;

    void constructor();

  public:
    const GameObjectType type = AUDIOSOURCE;

    AudioSourceTransform transform;

    AudioSource(Transform t) : GameObject(t) : transform(this);
    AudioSource() : GameObject() : transform(this);

    ~AudioSource();

    void SetSourceFloat(ALenum enum, float value);

    bool IsLooped();
    void SetLooping(bool loop);
    void PlayClip(AudioClip *clip);
};

#endif