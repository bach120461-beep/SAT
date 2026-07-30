#include "Shape.h"
#include "Physic.h"
#include <glm/gtc/constants.hpp>

std::shared_ptr<Mesh> MeshFactory::createCircle(float radius, int segments)
{
	std::vector<float> vertices;
	std::vector <unsigned int> indices;
	
	//Centre vertex
	vertices.insert(vertices.end(), { 0.0f,0.0f,0.0f });

	//Edge vertices
	for (int i = 0; i <= segments; i++)
	{
		float angle = (float)i / segments * glm::two_pi<float>();
		vertices.push_back(radius * cos(angle));
		vertices.push_back(radius * sin(angle));
		vertices.push_back(0.0f);
	}

	//Triangle fan indices
	for (int i = 0; i < segments; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}
	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::createCircleOutline(float radius, float thickness, int segments)
{
	std::vector<float> vertices;
	std::vector <unsigned int> indices;

	float innerRad = radius - thickness * 0.5f;
	float outerRad = radius + thickness * 0.5f;

	//Inner and outer vertices
	for (int i = 0; i <= segments; i++)
	{
		//These are just math
		float angle = (float)i / segments * glm::two_pi<float>();
		float cos_a = cos(angle);
		float sin_a = sin(angle);

		//Outer vertex
		vertices.push_back(outerRad * cos_a);
		vertices.push_back(outerRad * sin_a);
		vertices.push_back(0.0f);

		//Inner vertex
		vertices.push_back(innerRad * cos_a);
		vertices.push_back(innerRad * sin_a);
		vertices.push_back(0.0f);
	}
	//Circle segment order
	for (int i = 0; i < segments; i++)
	{
		unsigned int outer1 = i * 2;
		unsigned int inner1 = i * 2 + 1;
		unsigned int outer2 = (i + 1) * 2;
		unsigned int inner2 = (i + 1) * 2 + 1;

		indices.insert(indices.end(), {
			outer1, inner1, outer2,   // triangle 1
			inner1, inner2, outer2    // triangle 2
			});
	}
	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::createRectangle(float width, float height)
{
	float hw = width / 2.0f;
	float hh = height / 2.0f;

	std::vector<float> vertices = {
		-hw, -hh, 0.0f,  // bottom left
		 hw, -hh, 0.0f,  // bottom right
		 hw,  hh, 0.0f,  // top right
		-hw,  hh, 0.0f   // top left
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,   
		0, 2, 3    
	};

	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::createArrow(float length)
{
	float headSize = length * 0.2f;

	std::vector<float> vertices = {
		0.0f,        0.0f,      0.0f,  // tail
		length,      0.0f,      0.0f,  // tip
		length - headSize,  headSize,  0.0f,  // head top
		length - headSize, -headSize,  0.0f   // head bottom
	};

	std::vector<unsigned int> indices = {
		0, 1,   // shaft
		1, 2,   // head top
		1, 3    // head bottom
	};

	return std::make_shared<Mesh>(vertices, indices);
}
std::shared_ptr<Mesh> MeshFactory::createLine(glm::vec3 startPoint, glm::vec3 endPoint)
{
	std::vector<float> vertices = {
		startPoint.x, startPoint.y, startPoint.z, 
		endPoint.x, endPoint.y, endPoint.z
	};
	std::vector<unsigned int> indices = {
		0,1
	};
	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::createCar(float carW, float carH, float wheelR, int wheelSegs)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float hw = carW * 0.5f;
	float hh = carH * 0.5f;

	vertices.insert(vertices.end(), {
		// Body
		-hw, -hh, 0.0f, //0
		 hw, -hh, 0.0f, //1

		 hw, -hh * 0.1f, 0.0f, //2 front bumper
		 hw * 0.65f, hh * 0.1f,0.0f, //3 hood
		 hw * 0.25f, hh * 0.7f, 0.0f, //4 roof front
		-hw * 0.25f, hh *0.7f, 0.0f, //5 roof rear
		-hw * 0.65f, hh * 0.1f, 0.0f, //6 trunk
		-hw, -hh * 0.1f, 0.0f  //7 rear bumper
		});

	indices.insert(indices.end(), {
		0,1,2,
		0,2,7,
		7,2,6,
		2,3,6,
		6,3,5,
		3,4,5
	});

	// === Front wheel (circle) ===
	// offset = how many vertices already exist
	unsigned int offset = vertices.size() / 3;  // 4 so far

	// Wheel center offset from car center
	float wheelX = hw * 0.4f;   // 60% toward front
	float wheelY = -hh;          // bottom of car body

	// Center vertex of wheel
	vertices.insert(vertices.end(), { wheelX, wheelY, 0.0f });

	// Edge vertices
	for (int i = 0; i <= wheelSegs; i++) {
		float angle = (float)i / wheelSegs * glm::two_pi<float>();
		vertices.push_back(wheelX + wheelR * cos(angle));
		vertices.push_back(wheelY + wheelR * sin(angle));
		vertices.push_back(0.0f);
	}

	// Triangle fan indices (offset by existing vertex count)
	for (int i = 0; i < wheelSegs; i++) {
		indices.push_back(offset);          // center
		indices.push_back(offset + i + 1); // current edge
		indices.push_back(offset + i + 2); // next edge
	}

	// === Rear wheel (circle) ===
	offset = vertices.size() / 3;  // update offset

	float rearWheelX = -hw * 0.4f;  // 60% toward rear
	float rearWheelY = -hh;

	vertices.insert(vertices.end(), { rearWheelX, rearWheelY, 0.0f });

	for (int i = 0; i <= wheelSegs; i++) {
		float angle = (float)i / wheelSegs * glm::two_pi<float>();
		vertices.push_back(rearWheelX + wheelR * cos(angle));
		vertices.push_back(rearWheelY + wheelR * sin(angle));
		vertices.push_back(0.0f);
	}

	for (int i = 0; i < wheelSegs; i++) {
		indices.push_back(offset);
		indices.push_back(offset + i + 1);
		indices.push_back(offset + i + 2);
	}

	return std::make_shared<Mesh>(vertices, indices);
}

//Track - background scene blueprints
std::shared_ptr<Mesh> MeshFactory::createGround(float width, float thickness)
{
	float hw = width * 0.5f;
	float ht = thickness * 0.5f;

	std::vector<float> vertices = {
		-hw, -ht, 0.0f,
		 hw, -ht, 0.0f,
		 hw,  ht, 0.0f,
		-hw,  ht, 0.0f
	};
	std::vector<unsigned int> indices = { 0,1,2, 0,2,3 };
	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::createWall(float height, float thickness)
{
	float hh = height * 0.5f;
	float ht = thickness * 0.5f;

	std::vector<float> vertices = {
		-ht, -hh, 0.0f,
		ht, -hh, 0.0f,
		ht, hh, 0.0f,
		-ht, hh, 0.0f
	};
	std::vector <unsigned int> indices = { 0,1,2 , 0,2,3 };
	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr <Mesh> MeshFactory::createHalfCircle(float radius, int segments)
{
	std::vector<float> vertices;
	std::vector <unsigned int> indices;

	//Centre vertex
	vertices.insert(vertices.end(), { 0.0f,0.0f,0.0f });

	//Edge vertices
	for (int i = 0; i <= segments; i++)
	{
		float angle = (float)i / segments * glm::pi<float>();
		vertices.push_back(radius * cos(angle));
		vertices.push_back(radius * sin(angle));
		vertices.push_back(0.0f);
	}

	//Triangle fan indices
	for (int i = 0; i < segments; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}
	return std::make_shared<Mesh>(vertices, indices);
}
std::shared_ptr <Mesh> MeshFactory::createHalfCircleOutLine(float radius,float thickness, int segments)
{
	std::vector<float> vertices;
	std::vector <unsigned int> indices;

	float innerRad = radius - thickness * 0.5f;
	float outerRad = radius + thickness * 0.5f;

	//Inner and outer vertices
	for (int i = 0; i <= segments; i++)
	{
		//These are just math
		float angle = (float)i / segments * glm::pi<float>();
		float cos_a = cos(angle);
		float sin_a = sin(angle);

		//Outer vertex
		vertices.push_back(outerRad * cos_a);
		vertices.push_back(outerRad * sin_a);
		vertices.push_back(0.0f);

		//Inner vertex
		vertices.push_back(innerRad * cos_a);
		vertices.push_back(innerRad * sin_a);
		vertices.push_back(0.0f);
	}
	//Circle segment order
	for (int i = 0; i < segments; i++)
	{
		unsigned int outer1 = i * 2;
		unsigned int inner1 = i * 2 + 1;
		unsigned int outer2 = (i + 1) * 2;
		unsigned int inner2 = (i + 1) * 2 + 1;

		indices.insert(indices.end(), {
			outer1, inner1, outer2,   // triangle 1
			inner1, inner2, outer2    // triangle 2
			});
	}
	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::createArc(float radius, float thickness, float startAngle, float endAngle, int segments)
{
	std::vector<float> vertices;
	std::vector <unsigned int> indices;

	float innerRad = radius - thickness * 0.5f;
	float outerRad = radius + thickness * 0.5f;

	for (int i = 0; i <= segments; i++)
	{
		float angle = startAngle + (float)i / segments * (endAngle - startAngle);
		float cos_a = cos(angle);
		float sin_a = sin(angle);

		vertices.push_back(outerRad * cos_a);
		vertices.push_back(outerRad * sin_a);
		vertices.push_back(0.0f);

		vertices.push_back(innerRad * cos_a);
		vertices.push_back(innerRad * sin_a);
		vertices.push_back(0.0f);
	}
	for (int i = 0; i < segments; i++) {
		unsigned int outer1 = i * 2;
		unsigned int inner1 = i * 2 + 1;
		unsigned int outer2 = (i + 1) * 2;
		unsigned int inner2 = (i + 1) * 2 + 1;

		indices.insert(indices.end(), {
			outer1, inner1, outer2,
			inner1, inner2, outer2
			});
	}
	return std::make_shared<Mesh>(vertices, indices);
}

//Drawing boundary
Boundary& Boundary::addGround(float width, float thickness, glm::vec3 position, float friction)
{
	auto mesh = MeshFactory::createGround(width, thickness);

	BoundarySegment seg{
		SceneObject(mesh, position, glm::vec3(0.0f),std::numeric_limits<float>::infinity(),Color::Black),
		BoundaryType::FLAT,
		friction,
		0.0f,
		glm::vec3(0.0f)
	};
	segments.push_back(seg);
	return *this;
}
Boundary& Boundary::addWall(float height, float thickness, glm::vec3 position, float friction)
{
	auto mesh = MeshFactory::createWall(height, thickness);

	BoundarySegment seg{
		SceneObject(mesh, position, glm::vec3(0.0f),std::numeric_limits<float>::infinity(),Color::Black),
		BoundaryType::FLAT,
		friction,
		0.0f,
		glm::vec3(0.0f)
	};
	segments.push_back(seg);
	return *this;
}
Boundary& Boundary::addSlope(float width, float thickness, glm::vec3 position, float angle, float friction)
{
	auto mesh = MeshFactory::createGround(width, thickness);
	// Rotation baked into SceneObject
	SceneObject obj(mesh, position, glm::vec3(0.0f), std::numeric_limits<float>::infinity(), Color::Black);
	obj.physics.rotation = angle;  

	BoundarySegment seg{ obj, BoundaryType::SLOPE, friction, 0.0f, glm::vec3(0.0f) };
	segments.push_back(seg);
	return *this;
}
Boundary& Boundary::addCircularTrack(float radius, glm::vec3 center, float thickness, float friction, int segs)
{
	auto mesh = MeshFactory::createHalfCircleOutLine(radius, thickness, segs);

	BoundarySegment seg{
		SceneObject(mesh, center, glm::vec3(0.0f), std::numeric_limits<float>::infinity(),Color::Black),
		BoundaryType::CIRCULAR, friction, radius, center
	};
	segments.push_back(seg);
	return *this;
}
Boundary& Boundary::addLine(glm::vec3 startPoint, glm::vec3 endPoint, float thickness, float friction)
{
	// Calculate position, length, angle from start/end points
	glm::vec3 dir = endPoint - startPoint;
	float     length = glm::length(dir);
	float     angle = atan2(dir.y, dir.x);
	glm::vec3 center = (startPoint + endPoint) / 2.0f;

	auto mesh = MeshFactory::createGround(length, thickness);

	SceneObject obj(mesh, center, glm::vec3(0.0f),std::numeric_limits<float>::infinity(), Color::Black);
	obj.physics.rotation = angle;

	BoundarySegment seg{ obj, BoundaryType::FLAT, friction, 0.0f, glm::vec3(0.0f) };
	segments.push_back(seg);
	return *this;
}

Boundary& Boundary::addArc(float radius, float thickness, glm::vec3 position, float startAngle, float endAngle, float friction, int segs)
{
	auto mesh = MeshFactory::createArc(radius, thickness, startAngle, endAngle, segs);
	SceneObject obj(mesh, position, glm::vec3(0.0f), std::numeric_limits<float>::infinity(), Color::Black);
	BoundarySegment seg{ obj, BoundaryType::CIRCULAR, friction, radius, position };
	segments.push_back(seg);
	return *this;
}

void Boundary::draw(Renderer& renderer)
{
	for (auto& seg : segments) {
		renderer.drawMesh(*seg.object.mesh, seg.object.getModelMat(), seg.object.color);
	}
}
