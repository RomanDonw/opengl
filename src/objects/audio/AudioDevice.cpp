#include "AudioDevice.hpp"

#include <exception>
#include <string>
#include <sstream>

#include "AudioSystem.hpp"

// === AudioDeviceInitErrorException ===

/*AudioDeviceInitErrorException::AudioDeviceInitErrorException(std::string _detailed_error) : detailed_error(_detailed_error) {}

const char *AudioDeviceInitErrorException::what() { return detailed_error.c_str(); }*/

// === AudioDevice ===

AudioDevice::AudioDevice(const ALchar *devname)
{
    device = alcOpenDevice(devname);
    if (!device)
    {
        std::ostringstream oss;
        oss << "failed to open device \"" << devname << "\"";
        throw std::runtime_error(oss.str());
    }

    context = alcCreateContext(device, NULL);
    if (!context)
    {
        alcCloseDevice(device);

        std::ostringstream oss;
        oss << "failed to create context of device \"" << devname << "\"";
        throw std::runtime_error(oss.str());
    }
}

AudioDevice::~AudioDevice()
{
    if (AudioSystem::GetCurrentDevice() == this) AudioSystem::SetCurrentDevice(nullptr);

    alcDestroyContext(context);
    alcCloseDevice(device);
}