#pragma once

#include "PeterGameFramework/PeterGameFramework.h"
#include <SDL3/SDL_video.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3_image/SDL_image.h>


Engine::Engine()
{
    
}

bool Engine::Init(const char* windowTitle, int width, int height)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        char error[100];
        const char* sdlerror = SDL_GetError();
        strcpy(error, sdlerror);
        strcpy(error, "Failed to initialize SDL");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error, nullptr);
        return false;
    }


    mainState = SDLState();
    mainState.width = width;
    mainState.height = height;
    mainState.window = SDL_CreateWindow(windowTitle, width, height, SDL_WINDOW_RESIZABLE);
    
    if (!mainState.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialize window", nullptr);
        return false;
    }

    mainState.renderer = SDL_CreateRenderer(mainState.window, nullptr);

    if (!mainState.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialize renderer", nullptr);
        return false;
    }

    if (!SDL_SetRenderLogicalPresentation(mainState.renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to set render logical presentation", nullptr);
        return false;
    }

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); 

    float main_scale = SDL_GetWindowDisplayScale(mainState.window);
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    style.FontSizeBase = 12.0f;

    io.Fonts->AddFontDefault();

    ImGui_ImplSDL3_InitForSDLRenderer(mainState.window, mainState.renderer);
    ImGui_ImplSDLRenderer3_Init(mainState.renderer);

    return true;
}

bool Engine::RenderFrameStart()
{
    if (!mainState.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "No renderer found when calling RenderFrameStart, did you Init the engine?", nullptr);
        return false;
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    SDL_SetRenderDrawColor(mainState.renderer,0,0,0,255);
    SDL_RenderClear(mainState.renderer);
    return true;
}

bool Engine::RenderFrameEnd()
{
    if (!mainState.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "No renderer found when calling RenderFrameEnd, did you Init the engine?", nullptr);
        return false;
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mainState.renderer);
        
    SDL_RenderPresent(mainState.renderer);
    return true;
}

bool Engine::HandleEvent(SDL_Event* event, bool* running)
{
    if (!event)
    {
        return false;
    }

    ImGui_ImplSDL3_ProcessEvent(event);
    switch(event->type)
    {
        case SDL_EVENT_QUIT:
        {
            *running = false;
            return true;
        }
        case SDL_EVENT_WINDOW_RESIZED:
        {
            mainState.width = event->window.data1;
            mainState.height = event->window.data2;
            return true;
        }
    }

    return false;

}

std::shared_ptr<Actor> Engine::CreateActor(const char *textureFileName, SDL_FRect source, float w, float h, SDL_FRect Collision)
{
    std::shared_ptr<Actor> NewActor = std::make_shared<Actor>(this,textureFileName,source,w,h, Collision);
    AllActors.push_back(NewActor);
    return NewActor;
}

void Engine::Shutdown()
{
    AllActors.clear();
    SDL_DestroyRenderer(mainState.renderer);
    SDL_DestroyWindow(mainState.window);
    SDL_Quit();
}
