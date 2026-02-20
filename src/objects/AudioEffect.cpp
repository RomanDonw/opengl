#include "AudioEffect.hpp"

#include "AudioEffectSlot.hpp"

AudioEffect::AudioEffect(ALenum type)
{
    alGenEffects(1, &effect);
    alEffecti(effect, AL_EFFECT_TYPE, type);
}

AudioEffect::~AudioEffect()
{
    AttachToSlot(nullptr);
    
    alDeleteEffects(1, &effect);
}

void AudioEffect::SetEffectFloat(ALenum option, ALfloat value) { alEffectf(effect, option, value); }

AudioEffectSlot *AudioEffect::GetAttachedSlot() { return attached_slot; }
void AudioEffect::AttachToSlot(AudioEffectSlot *slot)
{
    if (attached_slot)
    {
        alAuxiliaryEffectSloti(attached_slot->slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
        attached_slot->attached_effect = nullptr;
    }
    if (slot)
    {
        alAuxiliaryEffectSloti(slot->slot, AL_EFFECTSLOT_EFFECT, effect);
        attached_slot->attached_effect = this;
    }

    attached_slot = slot;
}