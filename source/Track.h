#include "Shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

//This is the class that automatcally calculate value to build track
//Without this clas, a basic circular track looks like this
// .addGround(500.0f, 5.0f, glm::vec3(-250.0f, -200.0f, 0.0f))
//.addArc(40.0f, 5.0f, glm::vec3(0.0f, -160.0f, 0.0f), glm::radians(270.0f), glm::radians(330.0f))
//.addArc(100.0f, 5.0f, glm::vec3(120.5f, -230.0f, 0.0f), glm::radians(150.0f), glm::radians(30.0f))
//.addArc(40.0f, 5.0f, glm::vec3(242.0f, -160.0f, 0.0f), glm::radians(210.0f), glm::radians(270.0f));
// Which require me as the dev to manually calcuate position, start, end of each components of the track
// In short term, the boundary is the blue print of functions that create shape of the track
// this Track class is the constructor, the builder of that track

enum class TrackType
{
	LINE, ARC
};

struct TrackSegment //This one is recordin data from each components of the track
{
	TrackType type;

	//For line
	glm::vec3 start;
	glm::vec3 end;

	//For circular
	glm::vec3 center;
	float arcRad;
	float arcStart;
	float arcEnd;

	float friction;
};

class Track
{
private:
	// The track tip help keep track of the current endpoint of the track
	// The dir angle is the current angle of each components
	Boundary& boundary;
	glm::vec3 trackTip;
	float dirAngle;
public:
	std::vector<TrackSegment> segments;
	Track(Boundary& boundary, glm::vec3 startPoint = glm::vec3(0.0f), float initialAngle = 0.0f);
	//Method chaining will be used just as Boundary
	Track& addGround(float length, float thickness, float friction = 0.0f);
	//Both arc functions can be used to draw almost all same curve, the left one go anticlockwise, the right one is clockwise
	Track& addArcLeft(float radius, float angle, float thickness, float friction = 0.0f, int segs = 32); 
	Track& addArcRight(float radius, float angle, float thickness, float friction = 0.0f, int segs = 32);
	Track& addSlope(float length, float inclineAngle, float thickness, float friction = 0.0f);

};

struct RayHit
{
	bool hit = false;
	glm::vec3 point = glm::vec3(0.0f);
	glm::vec3 normal = glm::vec3(0.0f);
	float distance = FLT_MAX;
	float rad = 0.0f;
};

class TrackConstraint
{
public:
	Track& track;
	bool isActive;
	float normalForce;
	float groundAngle;

	RayHit castRay(glm::vec3 origin, glm::vec3 direction, float maxDist);
	void constrain(SceneObject& obj, float dt, float halfH);
};

