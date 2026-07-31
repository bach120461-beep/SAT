#include "Track.h"


Track::Track(Boundary& boundary, glm::vec3 startPoint, float initialAngle):
	boundary(boundary), trackTip(startPoint), dirAngle(glm::radians(initialAngle)) 
{}

Track& Track::addGround(float length, float thickness, float friction)
{
	glm::vec3 dir(cos(dirAngle), sin(dirAngle), 0.0f); //This just create a unit vector, don't worry about it, it's a specialist math thing
	glm::vec3 endPoint = trackTip + dir * length;

	boundary.addLine(trackTip, endPoint, thickness, friction);
	trackTip = endPoint; 
	return *this;
}

Track& Track::addArcLeft(float radius, float angle, float thickness, float friction, int segs)
{
	float angleRad = glm::radians(angle);

	//center is perpendicular to the left of the current direction
	float perpendicular = dirAngle + glm::half_pi<float>();
	glm::vec3 center = trackTip + glm::vec3(cos(perpendicular), sin(perpendicular), 0.0f) * radius;

	//Pass in boundary
	float startAngle = dirAngle - glm::half_pi<float>();
	float endAngle = startAngle + angleRad;

	boundary.addArc(radius, thickness, center, startAngle, endAngle, friction, segs);

	dirAngle += angleRad;
	trackTip = center + glm::vec3(cos(endAngle), sin(endAngle), 0.0f) * radius;
	return *this;
}
Track& Track::addArcRight(float radius, float angle, float thickness, float friction, int segs)
{
	float angleRad = glm::radians(angle);

	//center is perpendicular to the left of the current direction
	float perpendicular = dirAngle - glm::half_pi<float>();
	glm::vec3 center = trackTip + glm::vec3(cos(perpendicular), sin(perpendicular), 0.0f) * radius;

	//Pass in boundary
	float startAngle = dirAngle + glm::half_pi<float>();
	float endAngle = startAngle - angleRad;

	boundary.addArc(radius, thickness, center, startAngle, endAngle, friction, segs);

	dirAngle -= angleRad;
	trackTip = center + glm::vec3(cos(endAngle), sin(endAngle), 0.0f) * radius;
	return *this;
}
Track& Track::addSlope(float length, float inclineAngle, float thickness, float friction)
{
	float angleRad = glm::radians(inclineAngle);
	dirAngle += angleRad;

	return addGround(length, thickness, friction);
}