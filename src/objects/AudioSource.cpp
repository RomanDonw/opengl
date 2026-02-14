#include "AudioSource.hpp"

// === PRIVATE ===

void AudioSource::constructor()
{
    alGenSources(1, &source);

    SetLooping(false);
}

// === PUBLIC ===

AudioSource::AudioSource(Transform t) : GameObject(t), transform(this) { constructor(); }
AudioSource::AudioSource() : GameObject(), transform(this) { constructor(); }

AudioSource::~AudioSource() { alDeleteSources(1, &source); }

bool AudioSource::IsLooped() { return looped; }
void AudioSource::SetLooping(bool loop)
{
    looped = loop;
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void AudioSource::SetSourceFloat(ALenum option, float value) { alSourcef(source, option, value); }

void AudioSource::PlayClip(AudioClip *clip)
{
    alSourcei(source, AL_BUFFER, clip->buffer);

    alSourcePlay(source);
}