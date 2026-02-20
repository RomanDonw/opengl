#ifndef AUDIOEFFECT_HPP
#define AUDIOEFFECT_HPP

#include "../openal.hpp"

class AudioEffectSlot;

class AudioEffect
{
    private:
        ALuint effect;

        AudioEffectSlot *attached_slot = nullptr;

    public:
        AudioEffect();
        ~AudioEffect();

        void SetEffectType(ALenum type);
        void SetEffectFloat(ALenum option, ALfloat value);

        AudioEffectSlot *GetAttachedSlot();
        void AttachToSlot(AudioEffectSlot *slot);
};

#endif