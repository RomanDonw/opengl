#include "AudioSource.hpp"

#include <iostream>

// === PRIVATE ===

void AudioSource::constructor()
{
    alGenSources(1, &source);

    SetLooping(false);
}

void AudioSource::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();

    Transform globt = GetGlobalTransform();
    alSourcefv(source, AL_POSITION, glm::value_ptr(globt.GetPosition()));
    //std::cout << "called OnGlobalTransformChanged()" << std::endl;
}

// === PUBLIC ===

AudioSource::AudioSource(Transform t) : GameObject(t) { constructor(); }
AudioSource::AudioSource() : GameObject() { constructor(); }

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