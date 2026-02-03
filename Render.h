#pragma once
#include "Node.h"
#include <vector>
#include <imgui.h>

struct Scene {
    std::string name;
    std::vector<Node> nodes;
};

// Helpers
ImVec2 GetWorldPos(Scene& scene, Node& n);
void DrawNode(ImDrawList* dl, Node& n, ImVec2 pos);
void DrawSceneGrid(ImDrawList* dl, ImVec2 origin, ImVec2 size, float gridSize = 32.0f);
