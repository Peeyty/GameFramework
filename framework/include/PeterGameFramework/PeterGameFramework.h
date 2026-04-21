#pragma once
#include <SDL3/SDL.h>
#include "PeterGameFramework/FrameworkActor.h"
#include <vector>
#include <memory>

class SDLState
{
public:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width;
    int height;
};

class Engine
{
public:
    Engine();
    /**
     * Initalize the game framework and set up a basic window
     * \param state A pointer used to return the state, should be empty
     * \param windowTitle The title to give the window created by the initialization
     * \param width The width to give the window and state
     * \param height The height to give the window and state
     */
    bool Init(const char* windowTitle, int width, int height);
    bool RenderFrameStart();
    bool RenderFrameEnd();
    bool HandleEvent(SDL_Event* event, bool* running);
    std::shared_ptr<Actor> CreateActor(const char* textureFileName, SDL_FRect source = SDL_FRect{0,0,32,32}, float w = 32, float h = 32, SDL_FRect Collision = SDL_FRect{0,0,32,32});

    void Shutdown();
    
    SDLState mainState;
    std::vector<std::shared_ptr<Actor>> AllActors;

};

