#include "AudioSystem.hpp"

AudioDevice *AudioSystem::GetCurrentDevice() { return device; }
void AudioSystem::SetCurrentDevice(AudioDevice *dev)
{
    device = dev;
    alcMakeCurrentContext(device);
}