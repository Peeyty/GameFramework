#include "PeterGameFramework/FrameworkActor.h"
#include "PeterGameFramework/PeterGameFramework.h"
#include <string>

Actor::Actor(Engine *currentEngine, const char *textureFileName, SDL_FRect source, float w, float h, SDL_FRect Collision)
{
    if (currentEngine == nullptr)
    {
        return;
    }
    engine = currentEngine;
    std::string filepath = "../Assets/";
    filepath += textureFileName;
    texture = IMG_LoadTexture(engine->mainState.renderer, filepath.c_str());
    if (texture == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    SourceRect = source;
    CollisionBox = Collision;
    if (CollisionBox.h == 0 || CollisionBox.w == 0)
    {
        HasCollision = false;
    }
    RenderSize.x = w;
    RenderSize.y = h;
}

Actor::~Actor()
{
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
    }
}

void Actor::Render()
{
    if (texture == nullptr)
    {
        return;
    }

    SDL_FRect DestinationRect{ Position.x, Position.y, RenderSize.x, RenderSize.y};

    SDL_RenderTexture(engine->mainState.renderer, texture, &SourceRect, &DestinationRect);

}

void Actor::Update(float DeltaTime)
{
    if (!CanMove || DeltaTime > 0.01)
    {
        return;
    }
    if (HasGravity)
    {
        Velocity.y += 2000.0f * DeltaTime;
        Velocity.y = glm::min(Velocity.y, 1000.0f);
    }
    Position += Velocity * DeltaTime;
    if (Velocity.y != 0)
    {
        Grounded = false;
    }
    if (HasCollision)
    {
        if (engine)
        for (std::shared_ptr<Actor> other : engine->AllActors)
        {
            if (other && other->HasCollision && other.get() != this)
            {
                SDL_FRect ThisCollision{Position.x + CollisionBox.x, Position.y + CollisionBox.y, CollisionBox.w, CollisionBox.h};
                SDL_FRect OtherCollision{other->Position.x + other->CollisionBox.x, other->Position.y + other->CollisionBox.y, other->CollisionBox.w, other->CollisionBox.h};
                SDL_FRect Result;
                if (SDL_GetRectIntersectionFloat(&ThisCollision, &OtherCollision, &Result))
                {
                    if (abs(Result.h - Result.w) < 0.5)
                    {
                        continue;
                    }
                    if (Result.h < Result.w)
                    {
                        Position.y += glm::sign(Velocity.y) * Result.h * -1;
                        if (glm::sign(Velocity.y) == 1)
                        {
                            Grounded = true;
                            Velocity.y = 0;
                        }
                        else
                        {
                            if (OtherCollision.y < Result.y)
                            Velocity.y = 0;
                        }
                        
                    }
                    else
                    {
                        Position.x += glm::sign(Velocity.x) * Result.w * -1;
                        Velocity.x = 0;
                    }
                }
            }
        }
    }
}
