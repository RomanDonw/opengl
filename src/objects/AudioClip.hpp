#ifndef AUDIOCLIP_HPP
#define AUDIOCLIP_HPP

#include "../openal.hpp"

#include "AudioClip.hpp"

#include <string>
#include <vector>

class AudioSource;

class AudioClip
{
    friend class AudioSource;

    private:
        ALuint buffer;

        std::vector<AudioSource *> uses_sources = std::vector<AudioSource *>();
        void updatebuff(ALenum type, ALvoid *data, ALsizei size, ALsizei freq);

    public:
        AudioClip();
        ~AudioClip();

        bool LoadFromUCSOUNDFile(std::string filename);
};

#endif