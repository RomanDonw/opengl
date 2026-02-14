#include "AudioSourceTransform.hpp"

#include "AudioSource.hpp"

// === PRIVATE ===

void AudioSourceTransform::OnTransformUpdated()
{
    alSourcefv(source->source, AL_POSITION, GetPosition());
}

AudioSourceTransform::AudioSourceTransform() : Transform() : source(src) {};

// === PUBLIC ===

/*AudioSourceTransfrom::AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) : Transform(pos, rot, scl)
{ source = src; }
AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot) : Transform(pos, rot)
{ source = src; }
AudioSourceTransform(AudioSource *src, glm::vec3 pos) : Transform(pos)
{ source = src; }*/
/*AudioSourceTransform(AudioSource *src) : Transform();
{ source = src; }*/