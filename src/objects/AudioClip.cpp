#include "AudioClip.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <filesystem>

#include "AudioSource.hpp"

// === PRIVATE ===

void AudioClip::updatebuff(ALenum type, ALvoid *data, ALsizei size, ALsizei freq)
{
    for (AudioSource *src : uses_sources) src->Rewind();
    alBufferData(buffer, type, data, size, freq);
}

// === PUBLIC ===

AudioClip::AudioClip() { alGenBuffers(1, &buffer); }
AudioClip::~AudioClip()
{
    for (AudioSource *src : uses_sources) src->SetCurrentClip(nullptr);

    alDeleteBuffers(1, &buffer);
}

bool AudioClip::LoadFromUCSOUNDFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) return false;

    char sig[7];
    fread(&sig, sizeof(char), 7, f);
    if (feof(f) || strncmp(sig, "UCSOUND", 7)) { fclose(f); return false; }

    uint16_t version;
    fread(&version, sizeof(version), 1, f);
    if (feof(f) || version != 0) { fclose(f); return false; }

    uint8_t type;
    fread(&type, sizeof(type), 1, f);
    if (feof(f)) { fclose(f); return false; }

    ALenum altype;
    switch (type)
    {
        case 0: // mono 8 bit/sample (unsigned 8-bit).
            altype = AL_FORMAT_MONO8;
            break;

        case 1: // mono 16 bit/sample (signed 16-bit).
            altype = AL_FORMAT_MONO16;
            break;

        case 2: // stereo 8 bit/sample (unsigned 8-bit).
            altype = AL_FORMAT_STEREO8;
            break;

        case 3: // stereo 16 bit/sample (signed 16-bit).
            altype = AL_FORMAT_STEREO16;
            break;

        default:
            return false;
    }

    uint16_t frequency;
    fread(&frequency, sizeof(frequency), 1, f);
    if (feof(f)) { fclose(f); return false; }

    std::vector<uint8_t> data = std::vector<uint8_t>();
    while (true)
    {
        uint8_t byte;
        fread(&byte, sizeof(byte), 1, f);
        if (feof(f)) break;

        data.push_back(byte);
    }

    updatebuff(altype, data.data(), data.size(), frequency);

    return true;
}