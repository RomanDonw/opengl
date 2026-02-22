#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../opengl.hpp"

#include <string>

class Texture
{
    private:
        bool hasTexture = false;
        GLuint texture;

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