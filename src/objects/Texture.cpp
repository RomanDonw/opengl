#include "Texture.hpp"

#include <filesystem>
#include <cstring>
#include <iterator>

#include "Surface.hpp"

Texture::Texture() {}
Texture::~Texture()
{
    for (Surface *surface : attached_surfaces) surface->SetTexture(nullptr);

    DeleteTexture();
}

bool Texture::HasTexture() { return hasTexture; }
bool Texture::BindTexture()
{
    if (!HasTexture()) return false;

    glBindTexture(GL_TEXTURE_2D, texture);

    return true;
}

bool Texture::DeleteTexture()
{
    if (!HasTexture()) return false;

    glDeleteTextures(1, &texture);
    hasTexture = false;

    return true;
}

bool Texture::LoadFromUCTEXFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) return false;

    char sig[5];
    fread(&sig, sizeof(char), 5, f);
    if (feof(f) || strncmp(sig, "UCTEX", 5)) { fclose(f); return false; }

    uint16_t version;
    fread(&version, sizeof(version), 1, f);
    if (feof(f) || version != 0) { fclose(f); return false; }

    uint8_t type;
    fread(&type, sizeof(type), 1, f);
    if (feof(f) || (type > 3)) { fclose(f); return false; }

    uint16_t width16, height16;
    fread(&width16, sizeof(width16), 1, f);
    fread(&height16, sizeof(height16), 1, f);
    if (feof(f)) { fclose(f); return false; }
    uint32_t width = width16 + 1;
    uint32_t height = height16 + 1;

    DeleteTexture();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    hasTexture = true;

    bool texmiss_y = false;
    std::vector<uint32_t> pixels;
    switch (type)
    {
        case 0: // RGBA (0xAABBGGRR).
            for (uint32_t y = 0; y < height; y++)
            {
                bool texmiss_x = texmiss_y;
                for (uint32_t x = 0; x < width; x++)
                {
                    uint32_t pixel;
                    fread(&pixel, sizeof(uint32_t), 1, f);
                    if (feof(f))
                    {
                        if (texmiss_x) pixel = 0xFFFF00FF;
                        else pixel = 0xFF000000;
                    }
                    pixels.push_back(pixel);

                    texmiss_x = !texmiss_x;
                }
                texmiss_y = !texmiss_y;
            }
            break;

        case 1: // RGB (0xBBGGRR).
            for (uint32_t y = 0; y < height; y++)
            {
                bool texmiss_x = texmiss_y;
                for (uint32_t x = 0; x < width; x++)
                {
                    uint8_t r, g, b;
                    fread(&r, sizeof(uint8_t), 1, f);
                    fread(&g, sizeof(uint8_t), 1, f);
                    fread(&b, sizeof(uint8_t), 1, f);
                    if (feof(f))
                    {
                        if (texmiss_x) { r = 0; g = 255; b = 0; }
                        else { r = 255; g = 255; b = 255; }
                    }
                    pixels.push_back((0xFF << 24) | (b << 16) | (g << 8) | r);

                    texmiss_x = !texmiss_x;
                }
                texmiss_y = !texmiss_y;
            }
            break;

        case 2: // 16-bit depth RGBA (1 bit alpha). (0bABBBBBGGGGGRRRRR)
            for (uint32_t y = 0; y < height; y++)
            {
                bool texmiss_x = texmiss_y;
                for (uint32_t x = 0; x < width; x++)
                {
                    uint16_t pixel;
                    fread(&pixel, sizeof(uint16_t), 1, f);
                    if (feof(f))
                    {
                        if (texmiss_x) pixel = 0b1000001111111111;
                        else pixel = 0b1111110000000000;
                    }
                    pixels.push_back((((pixel >> 15) * 255) << 24) | ((pixel & 0b11111) << 3) | ((pixel & (0b11111 << 5)) << 6) | ((pixel & (0b11111 << 10)) << 9));

                    texmiss_x = !texmiss_x;
                }
                texmiss_y = !texmiss_y;
            }
            break;

        case 3: // 16-bit depth RGB (0b0BBBBBGGGGGRRRRR).
            for (uint32_t y = 0; y < height; y++)
            {
                bool texmiss_x = texmiss_y;
                for (uint32_t x = 0; x < width; x++)
                {
                    uint16_t pixel;
                    fread(&pixel, sizeof(uint16_t), 1, f);
                    if (feof(f))
                    {
                        if (texmiss_x) pixel = 0b0111111111100000;
                        else pixel = 0b0000000000011111;
                    }
                    pixels.push_back((0xFF << 24) | ((pixel & 0b11111) << 3) | ((pixel & (0b11111 << 5)) << 6) | ((pixel & (0b11111 << 10)) << 9));

                    texmiss_x = !texmiss_x;
                }
                texmiss_y = !texmiss_y;
            }
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    glBindTexture(GL_TEXTURE_2D, 0);
    fclose(f);

    return true;
}

bool Texture::SetTextureIntParameter(GLenum param, GLint value)
{
    if (!HasTexture()) return false;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, param, value);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture::SetDefaultParametres()
{
    SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SetTextureIntParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    SetTextureIntParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::SetLinearSmoothing()
{
    SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}