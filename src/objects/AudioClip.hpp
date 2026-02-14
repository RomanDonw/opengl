#ifndef AUDIOCLIP_HPP
#define AUDIOCLIP_HPP

#include "../openal.hpp"

#include <string>

class AudioClip
{
  private:
    ALuint buffer;

  public:
    AudioClip();
    ~AudioClip();

    bool LoadUCSOUNDFromFile(std::string filename);
};

#endif