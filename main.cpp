#include "Node.h"
#include "Render.h"
#include "stb_image.h"
#include "tinyfiledialogs.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <string>
#include <iostream>

// ===================== MAIN =====================
int main(int, char**) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Fox Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    SDL_GLContext gl = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl);
    ImGui_ImplOpenGL3_Init("#version 130");

    // ===================== STATE =====================
    std::vector<Scene> scenes;
    scenes.push_back({"Scene 1"});
    scenes.push_back({"Scene 2"});
    scenes[0].nodes.push_back({"Node0"});

    int currentScene = 0;
    int selectedNode = -1;
    bool uiLocked = true;
    bool running = true;
    int draggingNode = -1;
    ImVec2 dragOffset;
    ImGuiIO& io = ImGui::GetIO();

    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if(e.type == SDL_QUIT) running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // --------------------- CURRENT SCENE ---------------------
        Scene& scene = scenes[currentScene];

        // ===================== HIERARCHY =====================
        ImGui::Begin("Hierarchy");
        for(int i=0;i<(int)scene.nodes.size();i++) {
            Node& n = scene.nodes[i];
            if(ImGui::Selectable(n.name.c_str(), selectedNode==i))
                selectedNode = i;
        }
        if(ImGui::Button("Add Node")) {
            int id = scene.nodes.size();
            scene.nodes.push_back({"Node"+std::to_string(id)});
        }
        ImGui::End();

        // ===================== SCENE VIEW =====================
        ImGuiWindowFlags sceneFlags = uiLocked ? ImGuiWindowFlags_NoMove : 0;
        ImGui::Begin("Scene", nullptr, sceneFlags);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 origin = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetContentRegionAvail();

        DrawSceneGrid(dl, origin, size);

        for(int i=0;i<(int)scene.nodes.size();i++) {
            Node& n = scene.nodes[i];
            ImVec2 worldPos = GetWorldPos(scene,n);
            ImVec2 screenPos = origin + worldPos;

            DrawNode(dl,n,screenPos);

            // --------------------- DRAG ---------------------
            ImVec2 sz(n.width,n.height);
            bool hovered = ImGui::IsMouseHoveringRect(screenPos,screenPos+sz);
            if(selectedNode==i && hovered && ImGui::IsMouseClicked(0) && draggingNode==-1) {
                draggingNode = i;
                dragOffset = io.MousePos - screenPos;
            }
            if(draggingNode==i && ImGui::IsMouseDown(0)) {
                ImVec2 newPos = io.MousePos - dragOffset;
                n.x = newPos.x - origin.x;
                n.y = newPos.y - origin.y;
                if(n.parent>=0) {
                    ImVec2 parentWorld = GetWorldPos(scene, scene.nodes[n.parent]);
                    n.x -= parentWorld.x;
                    n.y -= parentWorld.y;
                }
            }
            if(draggingNode==i && ImGui::IsMouseReleased(0))
                draggingNode = -1;
        }
        ImGui::End();

        // ===================== SCENES PANEL =====================
        ImGui::Begin("Scenes Panel");
        if(ImGui::Button("Add Scene")) {
            scenes.push_back({"Scene " + std::to_string(scenes.size()+1)});
        }
        ImGui::SameLine();
        if (scenes.size() > 1 && ImGui::Button("Remove Scene")) {
            scenes.erase(scenes.begin() + currentScene);

            // Clamp scene index
            if (currentScene >= (int)scenes.size())
                currentScene = (int)scenes.size() - 1;

            // Reset selection safely
            selectedNode = -1;
        }

        if(ImGui::BeginTabBar("SceneTabs")) {
            for(int i=0;i<(int)scenes.size();i++) {
                if(ImGui::BeginTabItem(scenes[i].name.c_str())) {
                    currentScene = i;
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        // ===================== INSPECTOR =====================
        ImGui::Begin("Inspector");
        if(selectedNode>=0 && selectedNode < (int)scene.nodes.size()) {
            Node& n = scene.nodes[selectedNode];

            ImGui::InputText("Name",&n.name);
            ImGui::InputFloat("X",&n.x);
            ImGui::InputFloat("Y",&n.y);

            if(n.type != NODE_SOUNDPLAYER && n.type != NODE_LABEL) {
                ImGui::InputFloat("Width",&n.width);
                ImGui::InputFloat("Height",&n.height);
            }

            int typeIndex = (int)n.type;
            const char* types[] = { "Sprite","SoundPlayer","Button","Label" };
            if(ImGui::Combo("Type",&typeIndex,types,IM_ARRAYSIZE(types)))
                n.type = (NodeType)typeIndex;

            // --------------------- PARENTING ---------------------
            auto causesCycle = [&](int candidateParent) {
                int current = candidateParent;
                while(current != -1) {
                    if(current == selectedNode) return true;
                    if(current >= (int)scene.nodes.size()) break;
                    current = scene.nodes[current].parent;
                }
                return false;
            };

            std::vector<const char*> nodeNamesC = {"None"};
            std::vector<int> validParentIndices = {-1};

            for(int i=0;i<(int)scene.nodes.size();i++) {
                if(i==selectedNode) continue;
                if(causesCycle(i)) continue;
                nodeNamesC.push_back(scene.nodes[i].name.c_str());
                validParentIndices.push_back(i);
            }

            int comboIndex = 0;
            for(int j=0;j<(int)validParentIndices.size();j++)
                if(validParentIndices[j] == n.parent) { comboIndex = j; break; }

            if(ImGui::Combo("Parent",&comboIndex,nodeNamesC.data(),nodeNamesC.size()))
                n.parent = validParentIndices[comboIndex];

            // --------------------- TYPE-SPECIFIC ---------------------
            switch(n.type) {
                case NODE_SPRITE:
                    if(ImGui::Button(n.spriteFile.empty() ? "Choose Sprite" : n.spriteFile.c_str())) {
                        const char* patterns[] = {"*.png"};
                        const char* file = tinyfd_openFileDialog("Choose Sprite","",1,patterns,"PNG files",0);
                        if(file) n.LoadTexture(file);
                    }
                    break;
                case NODE_SOUNDPLAYER:
                    if(ImGui::Button(n.soundFile.empty() ? "Choose Sound" : n.soundFile.c_str())) {
                        const char* patterns[] = {"*.wav"};
                        const char* file = tinyfd_openFileDialog("Choose Sound","",1,patterns,"WAV files",0);
                        if(file) n.soundFile = file;
                    }
                    break;
                case NODE_BUTTON:
                    ImGui::InputText("Text",&n.buttonText);
                    ImGui::ColorEdit4("Color",&n.buttonColor[0]);
                    break;
                case NODE_LABEL:
                    ImGui::InputText("Text",&n.labelText);
                    ImGui::ColorEdit4("Color",&n.labelColor[0]);
                    break;
            }
        }

        ImGui::Checkbox("Unlock panels to drag and customize UI",&uiLocked);
        if(!uiLocked)
            ImGui::TextColored(ImVec4(1,0,0,1),"WARNING: Do NOT edit in this mode");
        ImGui::End();

        // ===================== RENDER =====================
        ImGui::Render();
        glViewport(0,0,1280,720);
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
