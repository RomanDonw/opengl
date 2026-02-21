#ifndef AUDIOSYSTEM_HPP
#define AUDIOSYSTEM_HPP

#include "../openal.hpp"

#include "AudioDevice.hpp"

class AudioSystem
{
    private:
        static AudioDevice *currdev;

    public:
        AudioSystem() = delete;

        static AudioDevice *GetCurrentDevice();
        static void SetCurrentDevice(AudioDevice *device);
};

#endif