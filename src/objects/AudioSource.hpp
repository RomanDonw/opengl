#ifndef AUDIOSOURCE_HPP
#define AUDIOSOURCE_HPP

#include "../openal.hpp"

#include "Transform.hpp"
#include "GameObject.hpp"

enum
{
    UNDEFINED = 0,
    INIT = 1,
    PLAYING = 2,
    PAUSED = 3,
    STOPPED = 4
} typedef AudioSourceState;

class AudioClip;

class AudioSource : public GameObject
{
    //friend class AudioClip;

    protected:
        ALuint source;
        AudioClip *currclip = nullptr;
        bool looped;

        void constructor();

        void updatesrcpos();

        void OnGlobalTransformChanged() override;

    public:
        const GameObjectType type = AUDIOSOURCE;

        AudioSource(Transform t);
        AudioSource();

        ~AudioSource() override;

        void SetSourceFloat(ALenum option, float value);

        bool IsLooped();
        void SetLooping(bool loop);

        AudioSourceState GetState();

        //bool CanClipBeChanged();
        AudioClip *GetCurrentClip();
        void SetCurrentClip(AudioClip *clip);

        void Play();
        void Pause();
        void Stop();
        void Rewind();
};

#endif