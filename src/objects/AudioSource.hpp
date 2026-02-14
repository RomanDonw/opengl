#ifndef AUDIOSOURCE_HPP
#define AUDIOSOURCE_HPP

#include "../openal.hpp"

#include "Transform.hpp"
#include "GameObject.hpp"
//#include "AudioSourceTransform.hpp"
#include "AudioClip.hpp"

class AudioSourceTransform;

class AudioSource : public GameObject
{
  private:
    friend class AudioClip;

    ALuint source;
    bool looped = false;

    void constructor();

  public:
    const GameObjectType type = AUDIOSOURCE;

    AudioSourceTransform transform;

    AudioSource(Transform t);
    AudioSource();

    ~AudioSource();

    void SetSourceFloat(ALenum option, float value);

    bool IsLooped();
    void SetLooping(bool loop);

    void PlayClip(AudioClip *clip);
};

#endif