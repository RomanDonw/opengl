#ifndef AUDIOEFFECT_HPP
#define AUDIOEFFECT_HPP

#include "../openal.hpp"

class AudioEffectSlot;

class AudioEffectProperties
{
    friend class AudioEffectSlot;

    private:
        ALuint effect;

    public:
        AudioEffectProperties();
        ~AudioEffectProperties();

        void SetEffectType(ALenum type);
        void SetEffectFloat(ALenum option, ALfloat value);
        void SetEffectInt(ALenum option, ALint value);
};

#endif