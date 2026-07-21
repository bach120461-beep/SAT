#include "Camera.h"

Camera::Camera(float viewportWidth, float viewportHeight)
    : position(0.0f, 0.0f, 0.0f), zoomLevel(1.0f),
    width(viewportWidth), height(viewportHeight)
{}

const glm::mat4 Camera::OrthoProjectionMat()
{
    float halfWidth = (width / 2.0f) / zoomLevel;
    float halfHeight = (height / 2.0f) / zoomLevel;

    return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
}

const glm::mat4 Camera::PerspectProjectionMat()
{
    float halfWidth = (width / 2.0f) / zoomLevel;
    float halfHeight = (height / 2.0f) / zoomLevel;

    return glm::perspective(glm::radians(45.0f), halfWidth / halfHeight, 0.1f, 100.0f);
}

const glm::mat4 Camera::getViewMat() {
    // Translate world so camera position becomes the origin
    return glm::translate(glm::mat4(1.0f), glm::vec3(-position));
}

void Camera::move(glm::vec3 delta) {
    position += delta;
}

void Camera::zoom(float factor) {
    zoomLevel *= factor; 
}