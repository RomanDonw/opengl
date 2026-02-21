#include "AudioEffectProperties.hpp"

AudioEffectProperties::AudioEffectProperties()
{
    alGenEffects(1, &effect);
}

AudioEffectProperties::~AudioEffectProperties()
{
    alDeleteEffects(1, &effect);
}

void AudioEffectProperties::SetEffectType(ALenum type) { alEffecti(effect, AL_EFFECT_TYPE, type); }
void AudioEffectProperties::SetEffectFloat(ALenum option, ALfloat value) { alEffectf(effect, option, value); }
void AudioEffectProperties::SetEffectInt(ALenum option, ALint value) { alEffecti(effect, option, value); }