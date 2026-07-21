#pragma once
#include <memory>
#include "Rendering/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Physicbody
{
public:
	Physicbody(glm::vec3 p_postion = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 p_velocity = glm::vec3(0.0f,0.0f,0.0f), float p_mass = 0);
	void applyForce(glm::vec3 p_force);
	void update(float deltaTime);
	float getSpeed();
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 accerleration;
	glm::vec3 force;
	float speed;
	float mass;
	float angularVelocity;
	float rotation;
};

struct CollisionInfo
{
	bool hit = false;
	glm::vec3 normal = glm::vec3(0.0f);
	float overlap = 0.0f;
};

class SceneObject
{
public:
	std::shared_ptr<Mesh> mesh;
	Physicbody physics;
	glm::vec4 color;
	glm::vec2 size;
	SceneObject(std::shared_ptr<Mesh> p_mesh
		, glm::vec3 p_postion = glm::vec3(0.0f, 0.0f, 0.0f)
		, glm::vec3 p_velocity = glm::vec3(0.0f, 0.0f, 0.0f)
		, float p_mass = 0
		, glm::vec4 p_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 getModelMat();
	void turnToVelocity();
	void calculateSize(const std::vector<float>& vertices);
	float calculateBoundingRadius() const;
	float restitution = 1.0f;
};

class Collision
{
public:
	//Handling collision logic
	static CollisionInfo checkAABB(const SceneObject& obj_one, const SceneObject& obj_two);
	static CollisionInfo checkCircle(const SceneObject& obj_one, const SceneObject& obj_two);
	static void resolveCollision(SceneObject& obj_one, SceneObject& obj_two, const CollisionInfo& info);
private:
	static void resolvePosition(SceneObject& a, SceneObject& b, const CollisionInfo& info);
	static void resolveVelocity(SceneObject& a, SceneObject& b, const CollisionInfo& info);

};

