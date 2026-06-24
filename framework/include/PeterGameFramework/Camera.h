#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "PeterGameFramework/FrameworkActor.h"
#include <memory>

class Camera
{

public:
    Camera();
    Camera(glm::vec2 size);

    void Update();
    glm::vec2 Position = glm::vec2(0,0);
    glm::vec4 CameraLimits = glm::vec4(0,0,0,0);
    glm::vec2 CameraSize = glm::vec2(800,800);
    std::shared_ptr<Actor> ToFollowActor;
};
