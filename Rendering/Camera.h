#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(float viewportWidth, float viewportHeight);

	const glm::mat4 OrthoProjectionMat();
	const glm::mat4 PerspectProjectionMat();
	const glm::mat4 getViewMat();

	void move(glm::vec3 delta);
	void zoom(float factor);

private:
	glm::vec3 position;
	float zoomLevel;
	float width;
	float height;
};