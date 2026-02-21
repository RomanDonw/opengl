#include "AudioEffectSlot.hpp"

#include <algorithm>

#include "AudioEffectProperties.hpp"
#include "AudioSource.hpp"

AudioEffectSlot::AudioEffectSlot()
{
    alGenAuxiliaryEffectSlots(1, &slot);
}

AudioEffectSlot::~AudioEffectSlot()
{
    for (AudioSource *src : attached_sources) RemoveSource(src);
    alDeleteAuxiliaryEffectSlots(1, &slot);
}

void AudioEffectSlot::ApplyEffect(AudioEffectProperties effect) { alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect.effect); }

bool AudioEffectSlot::HasAttachedSource(AudioSource *source) { return std::count(attached_sources.begin(), attached_sources.end(), source) > 0; } //{ return attached_sources.contains(source); }
std::vector<AudioSource *> AudioEffectSlot::GetAttachedSources() { return attached_sources; }
bool AudioEffectSlot::AddSource(AudioSource *source)
{
    if (HasAttachedSource(source)) return false;

    source->attached_slot = this;
    attached_sources.push_back(source);
    alSource3i(source->source, AL_AUXILIARY_SEND_FILTER, slot, 0, AL_FILTER_NULL);

    return true;
}
bool AudioEffectSlot::RemoveSource(AudioSource *source)
{
    if (!HasAttachedSource(source)) return false;

    source->attached_slot = nullptr;
    attached_sources.erase(std::remove(attached_sources.begin(), attached_sources.end(), source), attached_sources.end());
    alSource3i(source->source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);

    return true;
}