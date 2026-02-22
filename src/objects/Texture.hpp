#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../opengl.hpp"

#include <vector>
#include <string>

class Surface;

class Texture
{
    friend class Surface;

    private:
        bool hasTexture = false;
        GLuint texture;

        std::vector<Surface *> attached_surfaces = std::vector<Surface *>();

    public:
        Texture();
        ~Texture();

        //Texture Copy() { return *this; }

        bool HasTexture();
        bool BindTexture();

        bool DeleteTexture();

        bool LoadFromUCTEXFile(std::string filename);

        bool SetTextureIntParameter(GLenum param, GLint value);
        void SetDefaultParametres();
        void SetLinearSmoothing();
};

#endif