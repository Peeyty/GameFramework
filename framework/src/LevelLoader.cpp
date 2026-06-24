#include "PeterGameFramework/LevelLoader.h"
#include "PeterGameFramework/PeterGameFramework.h"
#include <sstream>

UniqueLevelObject::UniqueLevelObject()
{
    id = -1;
}

UniqueLevelObject::UniqueLevelObject(int spriteSheetId)
{
    id = spriteSheetId;
}

LevelLoader::LevelLoader()
{
    SpriteSheetPath = "";
    SpriteSize = glm::vec2(32);
    ActorSize = glm::vec2(32);
    Spacing = 0;
    engine = nullptr;
}

LevelLoader::LevelLoader(Engine* currentEngine, std::string SheetName, glm::vec2 spriteSize, glm::vec2 actorSize, float Space)
{

    if (currentEngine == nullptr)
    {
        engine = nullptr;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Nullptr engine passed to levelloader", nullptr);
        return;
    }

    engine = currentEngine;

    SpriteSize = spriteSize;
    ActorSize = actorSize;
    Spacing = Space;

    std::string SheetPath = "../Assets/" + SheetName;
    SDL_PathInfo Info;
    if (SDL_GetPathInfo(SheetPath.c_str(), &Info))
    {
        SpriteSheetPath = SheetPath;
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
    }
}

void LevelLoader::LoadMap(std::string fileName)
{
    if (engine == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Engine is null during load map", nullptr);
        return;
    }

    std::string FullFilePath = "../Assets/" + fileName;
    SDL_PathInfo Info;
    if (SDL_GetPathInfo(FullFilePath.c_str(), &Info))
    {
        if (SDL_GetPathInfo(SpriteSheetPath.c_str(), &Info))
        {
            for (std::shared_ptr<Actor> actor: engine->AllActors)
            {
                actor->~Actor();
            }
            engine->AllActors.clear();

            SDL_Surface* fullSpriteSheet = IMG_Load(SpriteSheetPath.c_str());

            if (fullSpriteSheet == nullptr)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
                return;
            }

            void* data = SDL_LoadFile(FullFilePath.c_str(), NULL);

            std::string TextData = static_cast<char*>(data);
            std::string line = "";
            std::istringstream iss(TextData);
            glm::vec2 Pos(0);

            while(std::getline(iss, line))
            {
                std::istringstream lineiss(line);
                std::string SingleNumber = "";
                while (std::getline(lineiss, SingleNumber, ','))
                {
                    int number = std::stoi(SingleNumber);
                    if (number != -1)
                    {
                        int XOfSprite = number * (SpriteSize.x + Spacing);
                        int YOfSprite = 0;
                        while (XOfSprite >= fullSpriteSheet->w)
                        {
                            XOfSprite -= fullSpriteSheet->w + 1;
                            YOfSprite += SpriteSize.y + Spacing;
                        }

                        if (YOfSprite >= fullSpriteSheet->h)
                        {
                            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Index of tile in map too big for spritesheet", nullptr);
                            return;
                        }

                        SDL_Surface* SpriteSurface = SDL_CreateSurface(ActorSize.x, ActorSize.y, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA64);

                        SDL_Rect src{XOfSprite, YOfSprite, SpriteSize.x, SpriteSize.y};

                        SDL_BlitSurfaceScaled(fullSpriteSheet, &src, SpriteSurface, NULL, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);

                        std::shared_ptr<Actor> NewActor;

                        std::vector<std::shared_ptr<UniqueLevelObject>>::iterator it = std::find_if(UniqueObjects.begin(), UniqueObjects.end(), [number](std::shared_ptr<UniqueLevelObject> object) {return object->id == number;});
                        if (it != UniqueObjects.end())
                        {
                            NewActor = it->get()->UniqueActorType(engine, SpriteSurface, ActorSize.x, ActorSize.y, SDL_FRect{0, 0, ActorSize.x, ActorSize.y});
                            
                            if (NewActor == nullptr)
                            {
                                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Level loader encountered a unqiue object with improper unique actor type", nullptr);
                            }
                            
                            it->get()->UniqueSetUp(NewActor);
                        }
                        else
                        {
                           NewActor = std::make_shared<Actor>(engine, SpriteSurface, ActorSize.x, ActorSize.y, SDL_FRect{0, 0, ActorSize.x, ActorSize.y});
                        }

                        engine->AllActors.push_back(NewActor);
                        NewActor->Position = glm::vec2(ActorSize.x * Pos.x, ActorSize.y * Pos.y);

                        SDL_free(SpriteSurface);


                    }
                    Pos.x += 1;
                }
                Pos.x = 0;
                Pos.y += 1;
            }

            SDL_free(data);
        }
        else
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
        }
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr);
    }
}
