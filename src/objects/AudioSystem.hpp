#ifndef AUDIOSYSTEM_HPP
#define AUDIOSYSTEM_HPP

#include "../openal.hpp"

//#include "AudioContext.hpp"
//#include "AudioDevice.hpp"



class AudioSystem
{
    private:
        static AudioDevice *device = nullptr;

    public:
        AudioSystem() = delete;

        static AudioDevice *GetCurrentDevice();
        static void SetCurrentDevice(AudioDevice *dev);
};

#endif