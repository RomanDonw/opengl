#ifndef TESTENTITIES_HPP
#define TESTENTITIES_HPP

Mesh buttonmesh = Mesh();

Texture buttontoggletex_on = Texture();
Texture buttontoggletex_off = Texture();

void inittestents()
{
    if (buttonmesh.LoadFromUCMESHFile("models/buttons/4.ucmesh")) std::cout << "loaded \"models/buttons/4.ucmesh\"" << std::endl;

    if (buttontoggletex_on.LoadFromUCTEXFile("textures/button/4_on.uctex")) std::cout << "loaded \"textures/button/4_on.uctex\"" << std::endl;
    if (buttontoggletex_off.LoadFromUCTEXFile("textures/button/4_off.uctex")) std::cout << "loaded \"textures/button/4_off.uctex\"" << std::endl;

    buttontoggletex_on.SetDefaultParametres();
    buttontoggletex_off.SetDefaultParametres();

    buttontoggletex_on.SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    buttontoggletex_on.SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    buttontoggletex_off.SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    buttontoggletex_off.SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

class HL1ToggleButton : public Entity
{
    private:
        bool enabled = false;
        AudioSource src = AudioSource();

        

        void constructor(AudioClip *interaction_sfx)
        {
            src.SetParent(this, false);

            src.SetSourceFloat(AL_REFERENCE_DISTANCE, 0);
            src.SetSourceFloat(AL_MAX_DISTANCE, 4);
            //src->SetSourceFloat(AL_PITCH, 1.5f);
            src.SetSourceFloat(AL_GAIN, 2.0f);

            src.SetCurrentClip(interaction_sfx);

            surfaces.push_back(Surface(&buttonmesh, &buttontoggletex_off));
        }

        bool e_pressed = false;
        double last_interaction_time = -1;

        double interaction_cooldown = 1;

    public:
        HL1ToggleButton(Transform t, AudioClip *interaction_sfx) : Entity(t) { constructor(interaction_sfx); }
        HL1ToggleButton(AudioClip *interaction_sfx) : Entity() { constructor(interaction_sfx); }

        ~HL1ToggleButton()
        {
            //delete src;
        }

        inline bool IsEnabled() { return enabled; }
        void SetEnabled(bool enable)
        {
            enabled = enable;

            if (enabled) surfaces[0].SetTexture(&buttontoggletex_on);
            else surfaces[0].SetTexture(&buttontoggletex_off);

            //src->Rewind();
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
                    SetEnabled(!IsEnabled());

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