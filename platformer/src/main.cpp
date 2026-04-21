#include <SDL3/SDL_main.h>
#include <iostream>
#include "PeterGameFramework/PeterGameFramework.h"
#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

int main(int argc, char** arv)
{
    Engine engine = Engine();
    if (!engine.Init("Platformer", 1200, 900))
    {
        engine.Shutdown();
        return 1;
    }

    std::shared_ptr<Actor> player = engine.CreateActor("TestSpriteGuy.png", SDL_FRect{0,0,820,1049},50,64, SDL_FRect{0,0,50,64});
    float PlayerSpeed = 200;
    player->CanMove = true;
    
    bool ObstaclesArray[13][18]= {{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, false, false, true,  false, false, false, true,  true,  true,  false, false},
                                  {false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, true,  true,  true,  false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, true,  true,  true,  true,  false, false, false, false, false, false, false, false},
                                  {false, false, false, false, false, false, false, false, false, false, false, false, false, true,  true,  false, false, false},
                                  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
                                  {false, false, false, true,  true,  true,  false, false, false, false, false, false, false, false, false, false, false, false},
                                  {true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true}};

    for (int i = 0; i < 13 ; i++)
    {
        for (int j = 0; j < 18; j++)
        {
            if (ObstaclesArray[i][j])
            {
                std::shared_ptr<Actor> Obstacle = engine.CreateActor("TestSpriteBrick.png", SDL_FRect{0,0,1184,1184}, 64, 64, SDL_FRect{0,0,64,64});
                Obstacle->Position = {j * 64, 68 + i * 64};
            }
        }

    }

    bool PressedSpace = false;
    int counter = 0;
    bool WasNotGrounded = false;
    int counterGround = 0;

    Uint64 lastTick = 0;
    Uint64 currentTick = 0;
    float deltaTime;

    bool running = true;

    while(running)
    {
        lastTick = currentTick;
        currentTick = SDL_GetTicks();
        deltaTime = (currentTick - lastTick) / 1000.0f;

        if (PressedSpace)
        {
            counter += 1;
            if (counter > 100)
            {
                PressedSpace = false;
                counter = 0;
            }
        }

        if (player->Grounded == false)
        {
            WasNotGrounded = true;
        }
        else
        {
            counterGround += 1;
            if (counterGround > 100)
            {
                WasNotGrounded = false;
                counterGround = 0;
            }
        }

        SDL_Event event{0};
        while(SDL_PollEvent(&event))
        {
            if (!engine.HandleEvent(&event, &running))
            {
                // Run game specfic events
                switch (event.type)
                {
                    case SDL_EVENT_KEY_DOWN:
                    if (event.key.scancode == SDL_SCANCODE_SPACE)
                    {
                        PressedSpace = true;
                        if (player->Grounded)
                        {
                            player->Grounded = false;
                            player->Velocity.y = -1000;
                        }
                    }
                    break;
                }
            }
        }

        if (!engine.RenderFrameStart())
        {
            running = false;
            break;
        }

        const bool* key_states = SDL_GetKeyboardState(nullptr);
        int MoveHorizontal = 0;

        if (key_states[SDL_SCANCODE_D])
        {
            MoveHorizontal += 1;
        }
        if (key_states[SDL_SCANCODE_A])
        {
            MoveHorizontal -= 1;
        }

        player->Velocity.x = MoveHorizontal * PlayerSpeed;
        for (std::shared_ptr<Actor> object : engine.AllActors)
        {
            if (object)
            object->Update(deltaTime);
        }

        for (std::shared_ptr<Actor> object : engine.AllActors)
        {
            if (object)
            object->Render();
        }

        ImGui::Begin("Variables");
        ImGui::Text("Grounded is: %s", player->Grounded ? "true" : "false");
        ImGui::Text("Press Space is: %s", PressedSpace ? "true" : "false");
        ImGui::Text("Was Not Grounded is: %s", WasNotGrounded ? "true" : "false");
        ImGui::Text("Player Y velocity is: %f", player->Velocity.y);
        ImGui::End();

        if (!engine.RenderFrameEnd())
        {
            running = false;
            break;
        }

    }
    engine.Shutdown();
    return 0;
}