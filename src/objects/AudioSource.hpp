#ifndef AUDIOSOURCE_HPP
#define AUDIOSOURCE_HPP

#include "../openal.hpp"

#include "Transform.hpp"
#include "GameObject.hpp"
#include "AudioClip.hpp"

class AudioSource : public GameObject
{
    friend class AudioClip;

    protected:
        ALuint source;
        bool looped = false;

        void constructor();

        void OnGlobalTransformChanged() override;

    public:
        const GameObjectType type = AUDIOSOURCE;

        AudioSource(Transform t);
        AudioSource();

        ~AudioSource() override;

        void SetSourceFloat(ALenum option, float value);

        bool IsLooped();
        void SetLooping(bool loop);

        void PlayClip(AudioClip *clip);
};

#endif