#include "AudioSourceTransform.hpp"

// === PRIVATE ===

void Transform::OnTransformUpdated()
{
    alSourcefv(source->source, AL_POSITION, GetPosition());
}

// === PUBLIC ===

/*AudioSourceTransfrom::AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) : Transform(pos, rot, scl)
{ source = src; }
AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot) : Transform(pos, rot)
{ source = src; }
AudioSourceTransform(AudioSource *src, glm::vec3 pos) : Transform(pos)
{ source = src; }*/
/*AudioSourceTransform(AudioSource *src) : Transform();
{ source = src; }*/