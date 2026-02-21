#ifndef AUDIOEFFECTSLOT_HPP
#define AUDIOEFFECTSLOT_HPP

#include "../openal.hpp"

#include <vector>

#include "AudioEffectProperties.hpp"

class AudioSource;

class AudioEffectSlot
{
    friend class AudioSource;

    private:
        ALuint slot;
        
        std::vector<AudioSource *> attached_sources = std::vector<AudioSource *>();

    public:
        AudioEffectSlot();
        ~AudioEffectSlot();

        void ApplyEffect(AudioEffectProperties effect);

        bool HasAttachedSource(AudioSource *source);
        std::vector<AudioSource *> GetAttachedSources();
        bool AddSource(AudioSource *source);
        bool RemoveSource(AudioSource *source);
};

#endif