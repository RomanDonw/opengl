#ifndef AUDIOCLIP_HPP
#define AUDIOCLIP_HPP

#include "../openal.hpp"

#include "AudioClip.hpp"

#include <string>

class AudioClip
{
  private:
    ALuint buffer;

  public:
    friend class AudioSource;

    AudioClip();
    ~AudioClip();

    bool LoadUCSOUNDFromFile(std::string filename);
};

#endif