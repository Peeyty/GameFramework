#pragma once
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>


class Actor
{
    public:
    Actor(class Engine* currentEngine, const char* textureFileName, SDL_FRect source = SDL_FRect{0,0,32,32}, float w = 32, float h = 32, SDL_FRect Collision = SDL_FRect{0,0,32,32});
    ~Actor();
    SDL_Texture* texture;
    SDL_FRect SourceRect{0,0,32,32};

    glm::vec2 Position{0,0};
    glm::vec2 Velocity{0,0};
    glm::vec2 RenderSize{32,32};
    SDL_FRect CollisionBox{0,0,32,32};
    bool HasCollision = true;
    bool CanMove = false;
    bool HasGravity = true;
    bool Grounded = false;

    void Render();
    void Update(float DeltaTime);

    private:
    class Engine* engine;
};
