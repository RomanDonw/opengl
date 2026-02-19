#include "AudioSource.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>

#include "AudioClip.hpp"
#include "AudioEffectSlot.hpp"

// === PRIVATE ===

void AudioSource::constructor()
{
    alGenSources(1, &source);

    alSourcei(source, AL_BUFFER, 0);
    SetLooping(false);
}

void AudioSource::updatesrcpos()
{
    Transform globt = GetGlobalTransform();
    alSourcefv(source, AL_POSITION, glm::value_ptr(globt.GetPosition()));
}

void AudioSource::OnGlobalTransformChanged()
{
    GameObject::OnGlobalTransformChanged();
    updatesrcpos();
}

// === PUBLIC ===

AudioSource::AudioSource(Transform t) : GameObject(t) { constructor(); }
AudioSource::AudioSource() : GameObject() { constructor(); }

AudioSource::~AudioSource()
{
    SetCurrentClip(nullptr);
    if (attached_slot) attached_slot->RemoveSource(this);

    alDeleteSources(1, &source);
}

bool AudioSource::IsLooped() { return looped; }
void AudioSource::SetLooping(bool loop)
{
    looped = loop;
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void AudioSource::SetSourceFloat(ALenum option, float value) { alSourcef(source, option, value); }

AudioSourceState AudioSource::GetState()
{
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    switch (state)
    {
        case AL_INITIAL:
            return INIT;

        case AL_PLAYING:
            return PLAYING;

        case AL_PAUSED:
            return PAUSED;

        case AL_STOPPED:
            return STOPPED;
    }
    return UNDEFINED;
}

/*bool AudioSource::CanClipBeChanged()
{
    AudioSourceState state = GetState();
    return state != PLAYING && state != PAUSED;
}*/

AudioClip *AudioSource::GetCurrentClip() { return currclip; }

void AudioSource::SetCurrentClip(AudioClip *clip)
{
    if (clip == currclip) return;
    /*if (!CanClipBeChanged())*/ Rewind();

    if (currclip) currclip->uses_sources.erase(std::remove(currclip->uses_sources.begin(), currclip->uses_sources.end(), this), currclip->uses_sources.end());

    if (clip)
    {
        clip->uses_sources.push_back(this);
        alSourcei(source, AL_BUFFER, clip->buffer);
    }
    else alSourcei(source, AL_BUFFER, 0);

    currclip = clip;
}

void AudioSource::Play() { updatesrcpos(); alSourcePlay(source); }
void AudioSource::Stop() { alSourceStop(source); }
void AudioSource::Pause() { alSourcePause(source); }
void AudioSource::Rewind() { alSourceRewind(source); }