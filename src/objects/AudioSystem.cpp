#include "AudioSystem.hpp"

#include <exception>
#include <stdexcept>
//#include <string>
//#include <sstream>

// === PRIVATE ===

AudioDevice *AudioSystem::currdev = nullptr;

// === PUBLIC ===

AudioDevice *AudioSystem::GetCurrentDevice() { return currdev; }
void AudioSystem::SetCurrentDevice(AudioDevice *device)
{
    currdev = device;

    if (currdev)
    {
        alcMakeContextCurrent(currdev->context);

        if (alcIsExtensionPresent(currdev->device, "ALC_EXT_EFX") == AL_FALSE)
        {
            //std::ostringstream oss;
            //oss << "can't detect EFX extension on cur";
            //throw std::runtime_error(oss.str());
            throw std::runtime_error("can't detect EFX extension on current device");
        }

        if (!initEFX())
        {
            //std::ostringstream oss;
            //oss << "failed to initialize EFX extension on device \""  << currdev->devname << "\"";
            //throw std::runtime_error(oss.str());
            throw std::runtime_error("failed to initialize EFX extension on current device");
        }
    }
    else alcMakeContextCurrent(NULL);
}