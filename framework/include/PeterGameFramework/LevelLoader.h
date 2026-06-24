#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Engine;
class Actor;

class UniqueLevelObject
{
public:
    UniqueLevelObject();
    UniqueLevelObject(int spriteSheetId);
    int id;

    virtual void UniqueSetUp(std::shared_ptr<Actor> actor) = 0;
    virtual std::shared_ptr<Actor> UniqueActorType(Engine *currentEngine, SDL_Surface *surface, float w, float h, SDL_FRect Collision) = 0;
};

class LevelLoader
{

public:

    LevelLoader();
    LevelLoader(Engine* currentEngine, std::string SheetName, glm::vec2 spriteSize, glm::vec2 actorSize, float Space);

    std::string SpriteSheetPath;
    glm::vec2 SpriteSize;
    glm::vec2 ActorSize;
    float Spacing;
    Engine* engine;

    std::vector<std::shared_ptr<UniqueLevelObject>> UniqueObjects;

    void LoadMap(std::string fileName);
};
