#include "AudioEffect.hpp"

#include "AudioEffectSlot.hpp"

AudioEffect::AudioEffect()
{
    alGenEffects(1, &effect);
}

AudioEffect::~AudioEffect()
{
    AttachToSlot(nullptr);
    
    alDeleteEffects(1, &effect);
}

void AudioEffect::SetEffectType(ALenum type) { alEffecti(effect, AL_EFFECT_TYPE, type); }
void AudioEffect::SetEffectFloat(ALenum option, ALfloat value) { alEffectf(effect, option, value); }

AudioEffectSlot *AudioEffect::GetAttachedSlot() { return attached_slot; }
void AudioEffect::AttachToSlot(AudioEffectSlot *slot)
{
    if (slot == attached_slot) return;

    if (attached_slot)
    {
        alAuxiliaryEffectSloti(attached_slot->slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
        attached_slot->attached_effect = nullptr;
    }
    if (slot)
    {
        alAuxiliaryEffectSloti(slot->slot, AL_EFFECTSLOT_EFFECT, effect);
        slot->attached_effect = this;
    }

    attached_slot = slot;
}