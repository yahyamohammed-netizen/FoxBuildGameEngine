#include "Render.h"
#include <GL/gl.h>

// ===================== HELPERS =====================
ImVec2 GetWorldPos(Scene& scene, Node& n) {
    ImVec2 pos(n.x, n.y);
    if(n.parent >= 0 && n.parent < (int)scene.nodes.size()) {
        ImVec2 parentPos = GetWorldPos(scene, scene.nodes[n.parent]);
        pos += parentPos;
    }
    return pos;
}

// ===================== DRAW NODE =====================
void DrawNode(ImDrawList* dl, Node& n, ImVec2 pos) {
    ImVec2 size(n.width, n.height);

    switch(n.type) {
        case NODE_SPRITE:
            if(n.textureID)
                dl->AddImage((void*)(intptr_t)n.textureID, pos, pos + size);
            else
                dl->AddRectFilled(pos, pos + size, IM_COL32(150,200,150,255), 6);
            break;
        case NODE_SOUNDPLAYER:
            dl->AddRectFilled(pos, pos + size, IM_COL32(200,150,150,255), 6);
            break;
        case NODE_BUTTON:
            dl->AddRectFilled(pos, pos + size, ImGui::ColorConvertFloat4ToU32(ImVec4(n.buttonColor[0], n.buttonColor[1], n.buttonColor[2], n.buttonColor[3])), 6);
            dl->AddText(pos + ImVec2(8,8), IM_COL32_WHITE, n.buttonText.empty() ? n.name.c_str() : n.buttonText.c_str());
            break;
        case NODE_LABEL:
            dl->AddRectFilled(pos, pos + size, IM_COL32(180,180,180,255), 6);
            dl->AddText(pos + ImVec2(8,8), ImGui::ColorConvertFloat4ToU32(ImVec4(n.labelColor[0], n.labelColor[1], n.labelColor[2], n.labelColor[3])), n.labelText.empty() ? n.name.c_str() : n.labelText.c_str());
            break;
    }


}


// ===================== DRAW GRID =====================
void DrawSceneGrid(ImDrawList* dl, ImVec2 origin, ImVec2 size, float gridSize) {
    for(float x=0; x<size.x; x+=gridSize)
        dl->AddLine(ImVec2(origin.x+x, origin.y), ImVec2(origin.x+x, origin.y+size.y), IM_COL32(60,60,60,255));
    for(float y=0; y<size.y; y+=gridSize)
        dl->AddLine(ImVec2(origin.x, origin.y+y), ImVec2(origin.x+size.x, origin.y+y), IM_COL32(60,60,60,255));
}
