#ifndef AUDIODEVICE_HPP
#define AUDIODEVICE_HPP

#include "../../openal.hpp"

class AudioSystem;

class AudioDevice
{
    friend class AudioSystem;

    private:
        ALCdevice *device;
        ALCcontext *context;

    public:
        // devname can be NULL/nullptr.
        AudioDevice(const ALchar *devname);
        ~AudioDevice();
};

#endif