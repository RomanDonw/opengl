#ifndef AUDIOSOURCETRANSFORM_HPP
#define AUDIOSOURCETRANSFORM_HPP

#include "../glm.hpp"
#include "../openal.hpp"

//#include "AudioSource.hpp"
#include "Transform.hpp"
//#include "GameObject.hpp"

class AudioSource;

class AudioSourceTransform : public Transform
{
  private:
    AudioSource *source;
    friend class AudioSource;

    void OnTransformUpdated();

    AudioSourceTransform(AudioSource *src);

  public:
    /*AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) : Transform(pos, rot, scl);
    AudioSourceTransform(AudioSource *src, glm::vec3 pos, glm::vec3 rot) : Transform(pos, rot)
    AudioSourceTransform(AudioSource *src, glm::vec3 pos) : Transform(pos)*/
    
};

#endif