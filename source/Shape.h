#pragma once
#include "Rendering/Mesh.h"
#include <memory>
#include "Rendering/Renderer.h"
#include <glm/glm.hpp>
#include "Physic.h"


//Handful way to adjust color of object
struct Color
{
	static constexpr glm::vec4 Black{ 0.0f, 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 White{ 1.0f,1.0f,1.0f, 1.0f };
	static constexpr glm::vec4 Red{ 1.0f, 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Green{ 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Blue{ 0.0f, 0.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 Yellow{ 1.0f, 1.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Orange{ 1.0f, 0.5f, 0.0f, 1.0f };
	static constexpr glm::vec4 Clear{ 0.0f, 0.0f, 0.0f, 0.0f };
};

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
	static std::shared_ptr<Mesh> createWall(float height, float thickness);
	static std::shared_ptr<Mesh> createCircularTrack(float radius, float thickness, int segments = 64);
};


/*
One track might contain different component with different shapes - contains different segments
*/
class Boundary // The track itself
{
public:
	std::vector<BoundarySegment> segments;

	//The following part is something call method chaining in OOP
	// It allows methods and function of a class that focus on configurating the created object of that class be written in a more managable way
	// For example:
	//  track.addGround().addWall().addCircularTrack();
	// instead of
	//  track.addGround();
	//	track.addWall();
	//	track.addCircularTrack();
	// With that being said, there are others classes in this simulation that should be following method chaining, eg the ScenObject, 
	// but due to time limit and I only learn this after finishing other classes, method chaining will only be apply to boundary.
	Boundary& addGround(float width, float thickness, glm::vec3 position, float friction = 0.0f);
	Boundary& addWall(float height, float thickness, glm::vec3 position, float friction = 0.0f);
	Boundary& addSlope(float width, float thickness, glm::vec3 position, float angle, float friction = 0.0f);
	Boundary& addCircularTrack(float radius, float thickness, glm::vec3 center, float friction = 0.0f, int segs = 64);
	Boundary& addLine(glm::vec3 startPoint, glm::vec3 endPoint, float thickness, float friction = 0.0f);
	void draw(Renderer& renderer);
	void checkAndResolve(SceneObject& obj);
};


