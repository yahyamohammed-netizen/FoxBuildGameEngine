#include "Node.h"
#include "stb_image.h"
#include <GL/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


bool LoadTexturePNG(Node& n) {
    if(n.spriteFile.empty()) return false;

    int channels;
    unsigned char* data = stbi_load(n.spriteFile.c_str(), &n.texWidth, &n.texHeight, &channels, 4);
    if(!data) return false;

    if(n.textureID != 0) glDeleteTextures(1, &n.textureID);

    glGenTextures(1, &n.textureID);
    glBindTexture(GL_TEXTURE_2D, n.textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, n.texWidth, n.texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return true;
}
