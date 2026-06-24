#include <SDL3/SDL_main.h>
#include <iostream>
#include "PeterGameFramework/PeterGameFramework.h"
#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

class LevelEndActor : public Actor
{
public:
    LevelEndActor(Engine *currentEngine, SDL_Surface *surface, float w, float h, SDL_FRect Collision) : Actor(currentEngine, surface, w, h, Collision) {}
    bool Update(float DeltaTime) override 
    { 
        if (engine)
        for (std::shared_ptr<Actor> other : engine->AllActors)
        {
            if (other && engine->camera.ToFollowActor == other)
            {
                SDL_FRect ThisCollision{Position.x + CollisionBox.x, Position.y + CollisionBox.y, CollisionBox.w, CollisionBox.h};
                SDL_FRect OtherCollision{other->Position.x + other->CollisionBox.x, other->Position.y + other->CollisionBox.y, other->CollisionBox.w, other->CollisionBox.h};
                SDL_FRect Result;
                if (SDL_GetRectIntersectionFloat(&ThisCollision, &OtherCollision, &Result))
                {
                    engine->mapLoader.LoadMap("TestMap2.csv");

                    std::shared_ptr<Actor> player = engine->CreateActor("TestSpriteGuy.png", SDL_FRect{0, 0, 820, 1049}, 50, 64, SDL_FRect{0, 0, 50, 64});
                    player->CanMove = true;

                    engine->camera.ToFollowActor = player;
                    engine->camera.CameraLimits = glm::vec4(0, 0, 6000, 900);
                    return false;
                }
            }
        }
        return true;
    }
};

class LevelEnd : public UniqueLevelObject
{
public:
    LevelEnd(int spriteSheetId) : UniqueLevelObject(spriteSheetId) {}
    void UniqueSetUp(std::shared_ptr<Actor> actor) override;
    std::shared_ptr<Actor> UniqueActorType(Engine *currentEngine, SDL_Surface *surface, float w, float h, SDL_FRect Collision) override;
};

void LevelEnd::UniqueSetUp(std::shared_ptr<Actor> actor)
{
    actor->HasCollision = false;
}

std::shared_ptr<Actor> LevelEnd::UniqueActorType(Engine *currentEngine, SDL_Surface *surface, float w, float h, SDL_FRect Collision)
{
    return std::make_shared<LevelEndActor>(currentEngine, surface, w, h, Collision);;
}

class NoCollision : public UniqueLevelObject
{
public:
    NoCollision(int spriteSheetId) : UniqueLevelObject(spriteSheetId) {}
    void UniqueSetUp(std::shared_ptr<Actor> actor) override;
    std::shared_ptr<Actor> UniqueActorType(Engine *currentEngine, SDL_Surface *surface, float w, float h, SDL_FRect Collision) override;
};

void NoCollision::UniqueSetUp(std::shared_ptr<Actor> actor)
{
    actor->HasCollision = false;
}

std::shared_ptr<Actor> NoCollision::UniqueActorType(Engine *currentEngine, SDL_Surface *surface, float w, float h, SDL_FRect Collision)
{
    return std::make_shared<Actor>(currentEngine, surface, w, h, Collision);;
}

int main(int argc, char** arv)
{
    Engine engine = Engine();
    if (!engine.Init("Platformer", 1200, 900))
    {
        engine.Shutdown();
        return 1;
    }

    engine.mapLoader = LevelLoader(&engine,"TileSheetPlatformer.png", glm::vec2(16), glm::vec2(64), 1);
    std::shared_ptr<UniqueLevelObject> EndLevel = std::make_shared<LevelEnd>(LevelEnd(7));
    engine.mapLoader.UniqueObjects.push_back(EndLevel);
    engine.mapLoader.UniqueObjects.push_back(std::make_shared<NoCollision>(1));
    engine.mapLoader.UniqueObjects.push_back(std::make_shared<NoCollision>(13));
    engine.mapLoader.UniqueObjects.push_back(std::make_shared<NoCollision>(14));
    engine.mapLoader.UniqueObjects.push_back(std::make_shared<NoCollision>(15));
    engine.mapLoader.LoadMap("TestMap1.csv");

    std::shared_ptr<Actor> player = engine.CreateActor("TestSpriteGuy.png", SDL_FRect{0,0,820,1049},50,64, SDL_FRect{0,0,50,64});
    float PlayerSpeed = 200;
    player->CanMove = true;

    int Score = 0;

    engine.camera.ToFollowActor = player;
    engine.camera.CameraLimits = glm::vec4(0,0,6000,900);

    engine.PlayAudio("musicloop.mp3", 1.0f,true);

    Uint64 lastTick = 0;
    Uint64 currentTick = 0;
    float deltaTime;

    bool running = true;

    while(running)
    {
        lastTick = currentTick;
        currentTick = SDL_GetTicks();
        deltaTime = (currentTick - lastTick) / 1000.0f;

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
                        if (engine.camera.ToFollowActor->Grounded)
                        {
                            engine.camera.ToFollowActor->Grounded = false;
                            engine.camera.ToFollowActor->Velocity.y = -1000;
                            engine.PlayAudio("jump.mp3", 0.1f);
                        }
                    }
                    break;
                }
            }
        }

        if (!engine.RenderFrameStart(glm::vec3(90,146,255)))
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

        engine.camera.ToFollowActor->Velocity.x = MoveHorizontal * PlayerSpeed;

        engine.UpdateAndRenderAllActors(deltaTime);

        ImGui::Begin("Variables");
        ImGui::Text("Grounded is: %s", engine.camera.ToFollowActor->Grounded ? "true" : "false");
        ImGui::Text("Player Y velocity is: %f", engine.camera.ToFollowActor->Velocity.y);
        ImGui::End();

        engine.RenderText("Text for Font rendering test", glm::vec2(10,10));

        if (!engine.RenderFrameEnd())
        {
            running = false;
            break;
        }

    }
    engine.Shutdown();
    return 0;
}