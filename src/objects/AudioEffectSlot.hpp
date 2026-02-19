#ifndef AUDIOEFFECTSLOT_HPP
#define AUDIOEFFECTSLOT_HPP

#include "../openal.hpp"

#include <vector>

class AudioEffect;
class AudioSource;

class AudioEffectSlot
{
    friend class AudioEffect;
    friend class AudioSource;

    private:
        ALuint slot;
        
        AudioEffect *attached_effect = nullptr;
        std::vector<AudioSource *> attached_sources = std::vector<AudioSource *>();

    public:
        AudioEffectSlot();
        ~AudioEffectSlot();

        bool HasAttachedSource(AudioSource *source);
        bool AddSource(AudioSource *source);
        bool RemoveSource(AudioSource *source);
};

#endif