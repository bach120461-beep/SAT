#include "Track.h"


Track::Track(Boundary& boundary, glm::vec3 startPoint, float initialAngle):
	boundary(boundary), trackTip(startPoint), dirAngle(glm::radians(initialAngle)) 
{}

Track& Track::addGround(float length, float thickness, float friction)
{
	glm::vec3 dir(cos(dirAngle), sin(dirAngle), 0.0f); //This just create a unit vector, don't worry about it, it's a specialist math thing
	glm::vec3 endPoint = trackTip + dir * length;

	boundary.addLine(trackTip, endPoint, thickness, friction);

	TrackSegment seg{
		TrackType::LINE, trackTip, endPoint, glm::vec3(0.0f), 0.0f, 0.0f, 0.0f, friction
	};
	segments.push_back(seg);
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

	TrackSegment seg{
		TrackType::ARC, glm::vec3(0.0f), glm::vec3(0.0f), center, radius, startAngle, endAngle, friction
	};
	segments.push_back(seg);
	dirAngle += angleRad;
	trackTip = center + glm::vec3(cos(endAngle), sin(endAngle), 0.0f) * radius;
	return *this;
}
Track& Track::addArcRight(float radius, float angle, float thickness, float friction, int segs)
{
    float angleRad = glm::radians(angle);
    float perpendicular = dirAngle - glm::half_pi<float>();
    glm::vec3 center = trackTip + glm::vec3(cos(perpendicular), sin(perpendicular), 0.0f) * radius;

    // Ensure startAngle < endAngle for standard angle range checks
    float startAngle = dirAngle + glm::half_pi<float>() - angleRad;
    float endAngle = dirAngle + glm::half_pi<float>();

    boundary.addArc(radius, thickness, center, startAngle, endAngle, friction, segs);

    TrackSegment seg{
        TrackType::ARC, glm::vec3(0.0f), glm::vec3(0.0f), center, radius, startAngle, endAngle, friction
    };
    segments.push_back(seg);

    dirAngle -= angleRad;
    trackTip = center + glm::vec3(cos(dirAngle + glm::half_pi<float>()), sin(dirAngle + glm::half_pi<float>()), 0.0f) * radius;
    return *this;
}
Track& Track::addSlope(float length, float inclineAngle, float thickness, float friction)
{
	float angleRad = glm::radians(inclineAngle);
	dirAngle += angleRad;

	return addGround(length, thickness, friction);
}

float NormalizeAngle(float angle) {
    while (angle < 0.0f) angle += glm::two_pi<float>();
    while (angle >= glm::two_pi<float>()) angle -= glm::two_pi<float>();
    return angle;
}

RayHit TrackConstraint::castRay(glm::vec3 origin, glm::vec3 direction, float maxDist)
{
    RayHit best;

    for (auto& seg : track.segments) {
        if (seg.type == TrackType::LINE) {
            // Ray vs line segment intersection
            glm::vec3 segDir = seg.end - seg.start;
            glm::vec3 segNormal = glm::normalize(
            glm::vec3(-segDir.y, segDir.x, 0.0f));

            float denom = glm::dot(direction, segNormal);
            if (abs(denom) < 0.0001f) continue;  // parallel

            float t = glm::dot(seg.start - origin, segNormal) / denom;
            if (t < 0 || t > maxDist) continue; 

            glm::vec3 hitPoint = origin + direction * t;

            float segLen = glm::length(segDir);
            float alongSeg = glm::dot(hitPoint - seg.start,
                glm::normalize(segDir));
            if (alongSeg < 0 || alongSeg > segLen) continue;

            if (t < best.distance) {
                best.hit = true;
                best.point = hitPoint;
                best.normal = segNormal;
                best.distance = t;
            }
        }
        else { 
            glm::vec3 toOrigin = origin - seg.center;
            float a = glm::dot(direction, direction);
            float b = 2.0f * glm::dot(toOrigin, direction);
            float c = glm::dot(toOrigin, toOrigin)
                - seg.arcRad * seg.arcRad;

            float discriminant = b * b - 4 * a * c;
            if (discriminant < 0) continue;  // no intersection

            float sqrtD = sqrt(discriminant);
            float t1 = (-b - sqrtD) / (2.0f * a);
            float t2 = (-b + sqrtD) / (2.0f * a);

            float t = -1.0f;
            if (t1 >= 0.0f && t1 <= maxDist) {
                t = t1;
            }
            else if (t2 >= 0.0f && t2 <= maxDist) {
                t = t2;
            }

            if (t < 0.0f) continue;

            glm::vec3 hitPoint = origin + direction * t;

            // Check hit is within arc angle range
            glm::vec3 toHit = hitPoint - seg.center;
            float hitAngle = NormalizeAngle(atan2(toHit.y, toHit.x));
            float startA = NormalizeAngle(seg.arcStart);
            float endA = NormalizeAngle(seg.arcEnd);

            if (startA <= endA) 
            {
                if (hitAngle < startA || hitAngle > endA) continue;
            }
            else 
            { 
                if (hitAngle < startA && hitAngle > endA) continue;
            }

            glm::vec3 normal = glm::normalize(hitPoint - seg.center);
            if (glm::dot(normal, direction) > 0.0f) {
                normal = -normal;
            }
            if (t < best.distance) {
                best.hit = true;
                best.point = hitPoint;
                best.normal = normal;
                best.distance = t;
            }
        }
    }
    return best;
}

void TrackConstraint::constrain(SceneObject& obj, float dt, float halfH)
{
    glm::vec3 rayOrigin = obj.physics.position;
    glm::vec3 localDown = glm::vec3(-sin(groundAngle), -cos(groundAngle), 0.0f);
    float maxDist = halfH + 50.0f;  

    RayHit hit = castRay(rayOrigin, localDown, maxDist);

    if (!hit.hit) { //if car get out of track, adjust its angle to gravity
        isActive = false;
        if (obj.physics.getSpeed() > 10.0f) {
            obj.physics.rotation = atan2(obj.physics.velocity.y, obj.physics.velocity.x);
        }
        return;
    }
    obj.physics.position = hit.point + hit.normal * halfH;
    groundAngle = atan2(hit.normal.x, hit.normal.y);  
    glm::vec3 tangent = glm::vec3(-hit.normal.y, hit.normal.x, 0.0f);

    // Keep velocity along surface only
    float speed = glm::dot(obj.physics.velocity, tangent);
    obj.physics.velocity = tangent * speed;

    // Gravity along surface
    glm::vec3 gravity = glm::vec3(0.0f, -9.8f * obj.physics.mass, 0.0f);
    float gravTangent = glm::dot(gravity, tangent);
    float gravNormal = glm::dot(gravity, hit.normal);

    //Align car angle to the track
    groundAngle = atan2(hit.normal.x, hit.normal.y);
    obj.physics.rotation = -groundAngle;

    obj.physics.applyForce(tangent * gravTangent);

    //Friction on surface
    if (abs(speed) > 0.01f) {
        float friction = 0.1f;
        obj.physics.applyForce(tangent * -glm::sign(speed) * friction * abs(gravNormal));
    }

    float centripetal = 0.0f;
    if (hit.rad> 0.0f) {
        centripetal = obj.physics.mass * (speed * speed / hit.rad);
    }
    normalForce = centripetal + obj.physics.mass * 9.8f * glm::dot(hit.normal, glm::vec3(0, 1, 0));

    if (normalForce < 0.0f)
        isActive = false;
}