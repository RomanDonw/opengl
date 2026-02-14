#ifndef AUDIOSOURCETRANSFORM_HPP
#define AUDIOSOURCETRANSFORM_HPP

#include "../glm.hpp"

#include "AudioSource.hpp"
#include "Transform.hpp"

class AudioSourceTransform : public Transform
{
  private:
    AudioSource *source;
    friend class AudioSource;

    void OnTransformUpdated();

    AudioSourceTransform(AudioSource *src) : Transform() : source(src);

  public:
    /*AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) : Transform(pos, rot, scl);
    AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot) : Transform(pos, rot)
    AudioSourceTransform(AudioSource *src, glm::vec3 pos) : Transform(pos)*/
    
};

#endif