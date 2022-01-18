// from the private repo of Alp Cihan (https://github.com/alpcihan)
#pragma once


#include "glad/glad.h"
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

namespace Textures
{
    vector<unsigned int> loadTextures(const vector<string> & texture_files)
    {
        vector<unsigned int> textures;

        stbi_set_flip_vertically_on_load(true);
        
        for (int i = 0; i < texture_files.size(); i++)
        {
            string filename = texture_files[i];
            const char* ch = filename.c_str();
            int x, y, n;
            unsigned char *texture_data = stbi_load(ch, &x, &y, &n, 0);

            if (texture_data == NULL)
            {
                std::cout << "Texture " << filename << " failed to load." << std::endl;
                std::cout << "Error: " << stbi_failure_reason() << std::endl;
            }
            else
            {
                std::cout << "Texture " << filename << " is loadad, X: " << x << " Y: " << y << " N: " << n << std::endl;
            }

            GLuint texture;
            glGenTextures(1, &texture);

            if ((x * n) % 4 != 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0, GL_RGBA,
                x, y, 0, n == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenerateMipmap(GL_TEXTURE_2D);

            if ((x * n) % 4 != 0)
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            // Free the texture
            stbi_image_free(texture_data);

            textures.push_back(texture);
        }

        return textures;
    }
}
