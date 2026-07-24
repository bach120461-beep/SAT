#pragma one
#include "Rendering/Mesh.h"
#include <memory>
#include <glm/glm.hpp>

//Pointer act as blueprint 
class MeshFactory
{
public:
	static std::shared_ptr<Mesh> createCircle(float radius, int segments = 64);
	static std::shared_ptr<Mesh> createCircleOutline(float radius, int segments = 64);
	static std::shared_ptr<Mesh> createRectangle(float width, float height);
	static std::shared_ptr<Mesh> createArrow(float length);
	static std::shared_ptr<Mesh> createLine(glm::vec3 startPoint, glm::vec3 endPoint);
	static std::shared_ptr<Mesh> createCar(float carW, float carH, float wheelR, int wheelSegs = 32);
	static std::shared_ptr<Mesh> createVinh();
};