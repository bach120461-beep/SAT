#pragma once
#include "Rendering/Mesh.h"
#include <memory>
#include <glm/glm.hpp>

//Pointer act as blueprint for shapes
class MeshFactory
{
public:
	static std::shared_ptr<Mesh> createCircle(float radius, int segments = 64);
	static std::shared_ptr<Mesh> createCircleOutline(float radius, int segments = 64);
	static std::shared_ptr<Mesh> createRectangle(float width, float height);
	static std::shared_ptr<Mesh> createArrow(float length);
	static std::shared_ptr<Mesh> createLine(glm::vec3 startPoint, glm::vec3 endPoint);
	static std::shared_ptr<Mesh> createCar(float carW, float carH, float wheelR, int wheelSegs = 32);
	static std::shared_ptr<Mesh> createGround(float width, float thickness);
	//static std::shared_ptr<Mesh> createWall(float height, float thickness);
	//static std::shared_ptr<Mesh> createCircularTrack(float radius, float thickness, int segments = 64);
};

//Handful way to adjust color of object
struct Color 
{
	static constexpr glm::vec4 Black{ 0.0f, 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 White{ 1.0f,1.0f,1.0f, 1.0f };
	static constexpr glm::vec4 Red { 1.0f, 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Green { 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Blue { 0.0f, 0.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 Yellow { 1.0f, 1.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Orange{1.0f, 0.5f, 0.0f, 1.0f};
	static constexpr glm::vec4 Clear{ 0.0f, 0.0f, 0.0f, 0.0f };
};

