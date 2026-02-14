#include "AudioSource.hpp"

#include "AudioSourceTransform.hpp"

// === PRIVATE ===

void AudioSource::constructor()
{
    alGenSources(1, &source);

    transform.onTransformChangeCallbacks.push_back([this](Transform *t)
    {
        alSourcefv(source, AL_POSITION, glm::value_ptr(t->GetPosition()));
    });

    SetLooping(false);
}

// === PUBLIC ===

AudioSource::AudioSource(Transform t) : GameObject(t) : transform(this) { constructor(); }
AudioSource::AudioSource() : GameObject() : transform(this) { constructor(); }

AudioSource::~AudioSource() { alDeleteSources(1, &source); }

bool AudioSource::IsLooped() { return looped; }
void AudioSource::SetLooping(bool loop)
{
    looped = loop;
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void SetSourceFloat(ALenum option, float value) { alSourcef(source, enum, value); }

void AudioSource::PlayClip(AudioClip *clip)
{
    alSourcei(source, AL_BUFFER, clip->buffer);

    alSourcePlay(source);
}