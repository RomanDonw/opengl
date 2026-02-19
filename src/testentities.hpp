#ifndef TESTENTITIES_HPP
#define TESTENTITIES_HPP

struct
{
    Mesh *mesh;
    
    Texture *on_texture;
    Texture *off_texture;

    AudioClip *interaction_sfx;
    AudioClip *locked_sfx;
} typedef HL1ToggleButtonSettings;

class HL1ToggleButton : public Entity
{
    private:
        bool enabled = false;
        AudioSource src = AudioSource();
        HL1ToggleButtonSettings settings;

        void constructor(HL1ToggleButtonSettings _settings)
        {
            settings = _settings;

            src.SetParent(this, false);

            src.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
            src.SetSourceFloat(AL_MAX_DISTANCE, 4);
            src.SetSourceFloat(AL_GAIN, 2.0f);

            surfaces.push_back(Surface(settings.mesh, settings.off_texture));
        }

        bool e_pressed = false;
        double last_interaction_time = -1;

        double interaction_cooldown = 1;

    public:
        bool locked = false;

        std::function<void (HL1ToggleButton *)> OnClickCallback = nullptr;

        HL1ToggleButton(Transform t, HL1ToggleButtonSettings _settings) : Entity(t) { constructor(_settings); }
        HL1ToggleButton(HL1ToggleButtonSettings _settings) : Entity() { constructor(_settings); }

        ~HL1ToggleButton()
        {
            //delete src;
        }

        inline bool IsEnabled() { return enabled; }
        void SetEnabled(bool enable)
        {
            enabled = enable;

            if (enabled) surfaces[0].SetTexture(settings.on_texture);
            else surfaces[0].SetTexture(settings.off_texture);

            src.SetCurrentClip(settings.interaction_sfx);
            src.Play();
        }

        void Update(float delta, Transform *camtr)
        {
            if (!e_pressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                e_pressed = true;

                /*if (glfwGetTime() >= 3 + last_interaction_time)
                {

                    

                    last_interaction_time = glfwGetTime();
                }*/

                float dist = glm::length(camtr->GetPosition() - transform.GetPosition());

                if (glfwGetTime() >= interaction_cooldown + last_interaction_time && dist <= 1)
                {
                    if (!locked)
                    {
                        SetEnabled(!IsEnabled());
                        if (OnClickCallback) OnClickCallback(this);
                    }
                    else
                    {
                        src.SetCurrentClip(settings.locked_sfx);
                        src.Play();
                    }

                    last_interaction_time = glfwGetTime();
                }
            }
            else if (e_pressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) e_pressed = false;
        }

        inline double GetInteractionCooldown() { return interaction_cooldown; }
        void SetInteractionCooldown(double cooldown)
        {
            if (cooldown < 0) cooldown = 0;
            interaction_cooldown = cooldown;
        }
};

#endif