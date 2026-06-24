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

    if (!TTF_Init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to set up ttf", nullptr);
        return false;
    }

    font = TTF_OpenFont("../Assets/AlteixsansRegulardemo-E4j1n.otf", 24);

    if (font == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
        return false;
    }

    if (!MIX_Init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to set up audio", nullptr);
        return false;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (mixer == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
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

    camera = Camera(glm::vec2(width, height));

    AllActors.clear();

    return true;
}

bool Engine::RenderFrameStart(glm::vec3 BackgroundColor)
{
    if (!mainState.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "No renderer found when calling RenderFrameStart, did you Init the engine?", nullptr);
        return false;
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    SDL_SetRenderDrawColor(mainState.renderer,BackgroundColor.x,BackgroundColor.y,BackgroundColor.z,255);
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

bool Engine::RenderText(std::string text, glm::vec2 ScreenPosition)
{
    bool Success = false;
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), 0, SDL_Color{0,0,0});
    if (textSurface != nullptr)
    {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mainState.renderer, textSurface);
        if (textTexture != nullptr)
        {
            SDL_FRect destRec{ScreenPosition.x, ScreenPosition.y, (float)textSurface->w, (float)textSurface->h};
            Success = SDL_RenderTexture(mainState.renderer, textTexture, NULL, &destRec);
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
    return Success;
}
bool Engine::PlayAudio(std::string fileName, float volume, bool loop)
{
    std::string fullPath = "../Assets/" + fileName;
    MIX_Audio* audio = MIX_LoadAudio(mixer, fullPath.c_str(), false);
    if (audio == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to load audio file", nullptr);
        return false;
    }

    MIX_Track* track = MIX_CreateTrack(mixer);
    if (track == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create track", nullptr);
        return false;
    }

    MIX_SetTrackAudio(track,audio);

    SDL_PropertiesID options = SDL_CreateProperties();

    if (loop)
    {
        SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    }

    MIX_SetTrackGain(track, volume);

    MIX_PlayTrack(track, options);

    SDL_DestroyProperties(options);

    return true;
}

void Engine::UpdateAndRenderAllActors(float DeltaTime)
{
    for (std::shared_ptr<Actor> object : AllActors)
    {
        if (object)
        if (!object->Update(DeltaTime))
        {
            break;
        }
    }

    camera.Update();

    for (std::shared_ptr<Actor> object : AllActors)
    {
        if (object)
        object->Render();
    }
}

void Engine::Shutdown()
{
    TTF_CloseFont(font);
    AllActors.clear();
    SDL_DestroyRenderer(mainState.renderer);
    SDL_DestroyWindow(mainState.window);
    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
}
