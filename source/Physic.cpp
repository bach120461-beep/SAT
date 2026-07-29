#include "Physic.h"
#include <iostream>
#include <algorithm>


//Physicbody
Physicbody::Physicbody(glm::vec3 p_postion, glm::vec3 p_velocity, float p_mass)
	:position(p_postion)
	, velocity(p_velocity)
	, mass(p_mass)
	, accerleration(0.0f,0.0f,0.0f)
	, force(0.0f,0.0f,0.0f)
	, speed(0.0f)
	, angularVelocity(0.0f)
	, rotation(0.0f)
{}

void Physicbody::applyForce(glm::vec3 p_force)
{
	force += p_force;
}

void Physicbody::update(float deltaTime)
{
	if (mass > 0.0f)
		accerleration = force / mass;
	else
		accerleration = glm::vec3(0.0f);
	//velocity
	if (accerleration != glm::vec3(0.0f,0.0f, 0.0f))
		velocity += accerleration * deltaTime;
	position += velocity * deltaTime;
	rotation += angularVelocity * deltaTime;
	//reset force for next iteration of render loop
	force = glm::vec3(0.0f, 0.0f, 0.0f);
}
float Physicbody::getSpeed()
{
	speed = glm::length(velocity);
	return speed;
}

//Scene Object
SceneObject::SceneObject(std::shared_ptr<Mesh> p_mesh
	, glm::vec3 p_postion 
	, glm::vec3 p_velocity
	, float p_mass
	, glm::vec4 p_color
) : mesh(p_mesh), physics(p_postion, p_velocity, p_mass), color(p_color)
{
	calculateSize(mesh->vertices);
}

glm::mat4 SceneObject::getModelMat()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, physics.position);
	model = glm::rotate(model, physics.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	return model;
}

void SceneObject::turnToVelocity()
{
	if (physics.getSpeed() > 0.0f)
		physics.rotation = atan2(physics.velocity.y, physics.velocity.x);
}

void SceneObject::calculateSize(const std::vector<float>& vertices)
{
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;

	// Step by 3 (x, y, z per vertex)
	for (int i = 0; i < vertices.size(); i += 3) {
		float x = vertices[i];
		float y = vertices[i + 1];

		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
	}

	size = glm::vec2(maxX - minX, maxY - minY);
}

float SceneObject::calculateBoundingRadius() const
{
	float maxDist = 0.0f;
	for (int i = 0; i < mesh->vertices.size(); i += 3)
	{
		float dist = sqrt(mesh->vertices[i] * mesh->vertices[i]+ mesh->vertices[i+1] * mesh->vertices[i+1]);
		if (dist > maxDist)
			maxDist = dist;
	}
	return maxDist;
}

CollisionInfo Collision::checkAABB(const SceneObject& obj_one, const SceneObject& obj_two)
{
	CollisionInfo info;
	info.hit = false;

	glm::vec3 posOne = glm::vec3(obj_one.physics.position);
	glm::vec3 posTwo = glm::vec3(obj_two.physics.position);

	float overlapX = (obj_one.size.x / 2 + obj_two.size.x / 2) - abs(posOne.x - posTwo.x);
	float overlapY = (obj_one.size.y / 2 + obj_two.size.y / 2) - abs(posOne.y - posTwo.y);

	//No collision
	if (overlapX <= 0 || overlapY <= 0)
		return info;
	//Collide
	info.hit = true;
	if (overlapX < overlapY)
	{
		info.overlap = overlapX;
		info.normal = glm::vec3(posOne.x < posTwo.x ? 1.0f : -1.0f, 0.0f, 0.0f);
	}
	else
	{
		info.overlap = overlapY;
		info.normal = glm::vec3(0.0f, posOne.y < posTwo.y ? 1.0f : -1.0f, 0.0f);
	}
	return info;
}

CollisionInfo Collision::checkCircle(const SceneObject& obj_one, const SceneObject& obj_two)
{
	CollisionInfo info;
	info.hit = false;

	glm::vec3 posOne = glm::vec3(obj_one.physics.position);
	glm::vec3 posTwo = glm::vec3(obj_two.physics.position);

	float dist = glm::length(posTwo - posOne);
	float radiSum = obj_one.calculateBoundingRadius() + obj_two.calculateBoundingRadius();
	
	//No collision

	if (dist >= radiSum)
		return info;

	info.hit = true;
	info.overlap = radiSum - dist;
	info.normal = glm::normalize(posTwo - posOne);

	return info;
}

void Collision::resolveCollision(SceneObject& obj_one, SceneObject& obj_two, const CollisionInfo& info)
{
	if (!info.hit)
		return;
	std::cout << "Before: " << info.overlap << '\n';
	resolvePosition(obj_one, obj_two, info);
	resolveVelocity(obj_one, obj_two, info);
}

void Collision::resolvePosition(SceneObject& a, SceneObject& b, const CollisionInfo& info)
{
	float percent = 0.8f;
	float slop = 0.01f;

	float correction =
		std::max(info.overlap - slop, 0.0f) * percent;
	//float push = info.overlap / 2.0f;
	a.physics.position -= info.normal * correction * 0.5f;
	b.physics.position += info.normal * correction * 0.5f;
}

void Collision::resolveVelocity(SceneObject& a, SceneObject& b, const CollisionInfo& info)
{
	float invMassA = (a.physics.mass > 0.0f) ? 1.0f / a.physics.mass : 0.0f;
	float invMassB = (b.physics.mass > 0.0f) ? 1.0f / b.physics.mass : 0.0f;

	glm::vec3 relVel = b.physics.velocity - a.physics.velocity;

	float velAlongNormal = glm::dot(relVel, info.normal);
	if (velAlongNormal > -0.001f)
		return;

	float e = std::min(a.restitution, b.restitution);

	float j = -(1.0f + e) * velAlongNormal;
	j /= invMassA + invMassB;

	glm::vec3 impulse = j * info.normal;

	a.physics.velocity -= impulse * invMassA;
	b.physics.velocity += impulse * invMassB;
}

//Boundary Checking
CollisionInfo Collision::checkAABB_Boundary(const SceneObject& obj, const BoundarySegment& boundary)
{
	return checkAABB(obj, boundary.object);
}

CollisionInfo Collision::checkCircularBoundary(const SceneObject& obj, const BoundarySegment& boundary)
{
	CollisionInfo info;
	info.hit = false;

	glm::vec3 toObj = obj.physics.position - boundary.trackCenter;
	float dist = glm::length(toObj);

	// Object is outside the track ring
	float innerEdge = boundary.trackRad - 5.0f;  // inner wall
	float outerEdge = boundary.trackRad + 5.0f;  // outer wall

	if (dist < innerEdge || dist > outerEdge) {
		info.hit = true;
		info.normal = glm::normalize(toObj);  // push radially outward
		info.overlap = dist < innerEdge? innerEdge - dist: dist - outerEdge;
	}
	return info;
}

void Collision::resolveBoundaryCollision(SceneObject& obj, const BoundarySegment& boundary, const CollisionInfo& info)
{
	if (!info.hit) return;

	SceneObject& staticBoundaryObj = const_cast<SceneObject&>(boundary.object);

	// 2. Reuse standard object-to-object collision resolution!
	resolveCollision(obj, staticBoundaryObj, info);

	// 3. Apply simple tangential friction if sliding along the boundary
	glm::vec3 tangent = obj.physics.velocity - glm::dot(obj.physics.velocity, info.normal) * info.normal;
	float tangentSpeed = glm::length(tangent);

	if (tangentSpeed > 0.001f && boundary.friction > 0.0f)
	{
		// Simple friction damping along surface tangent
		obj.physics.velocity -= (tangent / tangentSpeed) * (boundary.friction * 0.1f);
	}
}