#ifndef AUDIOLISTENER_HPP
#define AUDIOLISTENER_HPP

#include "../GameObject.hpp"

class AudioListener : public GameObject
{
    private:
        static bool hasListener;

        void constructor();

        void OnGlobalTransformChanged() override;

    public:
        const GameObjectType type = AUDIOLISTENER;

        AudioListener(Transform t);
        AudioListener();

        ~AudioListener();
};

#endif