#pragma once
#include <string>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <stb_image.h>

// ===================== IMVEC2 OPERATORS =====================
inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2& operator+=(ImVec2& a, const ImVec2& b) { a.x += b.x; a.y += b.y; return a; }
inline ImVec2& operator-=(ImVec2& a, const ImVec2& b) { a.x -= b.x; a.y -= b.y; return a; }

// ===================== NODE TYPES =====================
enum NodeType {
    NODE_SPRITE,
    NODE_SOUNDPLAYER,
    NODE_BUTTON,
    NODE_LABEL
};

inline const char* NodeTypeName(NodeType t) {
    switch(t) {
        case NODE_SPRITE: return "Sprite";
        case NODE_SOUNDPLAYER: return "SoundPlayer";
        case NODE_BUTTON: return "Button";
        case NODE_LABEL: return "Label";
    }
    return "?";
}

// ===================== NODE =====================
struct Node {
    std::string name;
    NodeType type = NODE_SPRITE;
    float x = 0.0f, y = 0.0f;
    int parent = -1;

    // Type-specific
    std::string spriteFile;
    std::string soundFile;
    std::string buttonText;
    float width = 120.0f;
    float height = 50.0f;
    float buttonColor[4] = {0.6f,0.6f,0.6f,1.0f};
    std::string labelText;
    float labelColor[4] = {1,1,1,1};

    // OpenGL texture
    GLuint textureID = 0;
    int texWidth = 0;
    int texHeight = 0;

    Node() = default;

    void LoadTexture(const char* file) {
        if(textureID) glDeleteTextures(1,&textureID);

        int w,h,n;
        unsigned char* data = stbi_load(file,&w,&h,&n,4);
        if(!data) return;

        texWidth = w;
        texHeight = h;

        glGenTextures(1,&textureID);
        glBindTexture(GL_TEXTURE_2D,textureID);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D,0);

        stbi_image_free(data);
        spriteFile = file;
    }

    // Optional helper to get ImVec2 position
    ImVec2 GetPos() const { return ImVec2(x, y); }
    ImVec2 GetSize() const { return ImVec2(width, height); }
};
