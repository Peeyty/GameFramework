#include "PeterGameFramework/Camera.h"

Camera::Camera()
{

}

Camera::Camera(glm::vec2 size)
{
    CameraSize = size;
}

void Camera::Update()
{
    if (ToFollowActor == nullptr)
    {
        return;
    }

    if (CameraLimits.z - CameraLimits.x  < CameraSize.x || CameraLimits.w - CameraLimits.y < CameraSize.y)
    {
        Position = ToFollowActor->Position + (ToFollowActor->RenderSize / 2.0f) - (CameraSize / 2.0f);
    }
    else
    {
        Position = ToFollowActor->Position + (ToFollowActor->RenderSize / 2.0f) - (CameraSize / 2.0f);
        Position.x = glm::max(Position.x, CameraLimits.x);
        Position.y = glm::max(Position.y, CameraLimits.y);
        Position.x = glm::min(Position.x, CameraLimits.z - CameraSize.x);
        Position.y = glm::min(Position.y, CameraLimits.w - CameraSize.y);
    }
}