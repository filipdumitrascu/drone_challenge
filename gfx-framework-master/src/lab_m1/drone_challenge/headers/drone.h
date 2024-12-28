#ifndef DRONE_H
#define DRONE_H

#include "transforms3D.h"
#include "utils/glm_utils.h"

#include <cmath>

namespace drone
{
	inline glm::mat4 GeneratePropeller(glm::vec3 center, float propellerAngle)
	{
		return transforms3D::Translate(center.x, center.y, center.z) * transforms3D::RotateOY(propellerAngle);
	}

	inline glm::mat4 GenerateDrone(glm::vec3 dronePos, float pitchAngle, float yawAngle, float rollAngle)
	{
		return transforms3D::Translate(dronePos.x, dronePos.y, dronePos.z) * transforms3D::RotateOY(yawAngle) *
			transforms3D::RotateOX(pitchAngle)  * transforms3D::RotateOZ(rollAngle);
	}

	inline glm::mat4 GenerateArrow(glm::vec3 dronePos, glm::vec3 targetPos)
	{
		glm::vec3 direction = glm::normalize(targetPos - dronePos);
		float angle = atan2(direction.x, direction.z) + RADIANS(180.0f);

		return transforms3D::Translate(dronePos.x, dronePos.y + lit::arrowHeight, dronePos.z) * transforms3D::RotateOY(angle);
	}

	inline glm::mat4 GenerateIndicator(glm::vec3 dronePos, glm::vec2 zonePos)
	{
		glm::vec3 direction = glm::normalize(glm::vec3(zonePos.x, 1.0f, zonePos.y) - dronePos);
		float angle = atan2(direction.x, direction.z) + RADIANS(180.0f);

		return transforms3D::Translate(zonePos.x, 5.0f, zonePos.y) * transforms3D::RotateOY(angle);
	}

	inline float fieldNoise(glm::vec2 pos, float seed) {
		glm::vec2 i = glm::floor(pos);
		glm::vec2 j = glm::fract(pos);

		float a = glm::fract(sin(glm::dot(i, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f + seed);
		float b = glm::fract(sin(glm::dot(i + glm::vec2(1.0f, 0.0f), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f + seed);
		float c = glm::fract(sin(glm::dot(i + glm::vec2(0.0f, 1.0f), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f + seed);
		float d = glm::fract(sin(glm::dot(i + glm::vec2(1.0f, 1.0f), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f + seed);

		glm::vec2 u = j * j * (3.0f - 2.0f * j);
		return glm::mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
	}

	inline std::vector<float> DroneAABB(glm::vec3 droneCenter, int xoyTiltLvl, m1::PackageStatus status) {
		float distToEdge = (lit::droneBodyOX - lit::droneBodyOZ + lit::propellerOX) / 2.0f;

		float minZ{ droneCenter.z - distToEdge };
		float maxZ{ droneCenter.z + distToEdge };

		float minY{};
		float maxY{};

		if (xoyTiltLvl == 0) {
			minY = droneCenter.y;
			maxY = droneCenter.y + 2 * lit::droneBodyOY + lit::propellerOY;

		}
		else if (xoyTiltLvl == 1) {
			minY = droneCenter.y - distToEdge * sin(RADIANS(15.0f));
			maxY = droneCenter.y + distToEdge * sin(RADIANS(15.0f));

		}
		else {
			minY = droneCenter.y - distToEdge * sin(RADIANS(15.0f * sqrt(2.0f)));
			maxY = droneCenter.y + distToEdge * sin(RADIANS(15.0f * sqrt(2.0f)));
		}

		if (status == m1::PackageStatus::ATTACHED) {
			minY -= lit::packageSide;
		}

		float minX{ droneCenter.x - distToEdge };
		float maxX{ droneCenter.x + distToEdge };

		return {minX, maxX, minY, maxY, minZ, maxZ};
	}

	inline bool isDroneCollidingWithCube(glm::vec3 droneCenter, glm::vec3 cubeCenter,
		int xoyTiltLvl, float cubeSide, float scaleFactor, m1::PackageStatus status)
	{
		std::vector<float> droneAABB = DroneAABB(droneCenter, xoyTiltLvl, status);

		float minXDrone{ droneAABB[0] }, maxXDrone{ droneAABB[1] };
		float minYDrone{ droneAABB[2] }, maxYDrone{ droneAABB[3] };
		float minZDrone{ droneAABB[4] }, maxZDrone{ droneAABB[5] };

		float minXCube{ cubeCenter.x - cubeSide / 2.0f };
		float maxXCube{ cubeCenter.x + cubeSide / 2.0f };

		float minYCube{ cubeCenter.y };
		float maxYCube{ cubeCenter.y + cubeSide * scaleFactor };

		float minZCube{ cubeCenter.z - cubeSide / 2.0f };
		float maxZCube{ cubeCenter.z + cubeSide / 2.0f };

		return (minXDrone <= maxXCube && maxXDrone >= minXCube && minYDrone <= maxYCube &&
			maxYDrone >= minYCube && minZDrone <= maxZCube && maxZDrone >= minZCube);
	}

	inline bool isDroneCollidingWithPrism(glm::vec3 droneCenter, glm::vec3 prismCenter,
		int xoyTiltLvl, float scaleFactor, m1::PackageStatus status)
	{
		std::vector<float> droneAABB = DroneAABB(droneCenter, xoyTiltLvl, status);

		float minXDrone{ droneAABB[0] }, maxXDrone{ droneAABB[1] };
		float minYDrone{ droneAABB[2] }, maxYDrone{ droneAABB[3] };
		float minZDrone{ droneAABB[4] }, maxZDrone{ droneAABB[5] };

		float minYPrism = prismCenter.y;
		float maxYPrism = prismCenter.y + lit::roofHeight * scaleFactor;

		if (!(minYDrone <= maxYPrism && maxYDrone >= minYPrism)) {
			return false;
		}

		float diffHeight = glm::clamp(maxYDrone, minYPrism, maxYPrism) - prismCenter.y;
		float halfBaseSide = (lit::houseSide / 2.0f) * (1.0f - (diffHeight / (lit::roofHeight * scaleFactor)));

		float minXPrism = prismCenter.x - halfBaseSide;
		float maxXPrism = prismCenter.x + halfBaseSide;

		float minZPrism = prismCenter.z - halfBaseSide;
		float maxZPrism = prismCenter.z + halfBaseSide;

		return minXDrone <= maxXPrism && maxXDrone >= minXPrism &&
			minZDrone <= maxZPrism && maxZDrone >= minZPrism;
	}

	inline bool isDroneCollidingWithCylinder(glm::vec3 droneCenter, glm::vec3 cylinderCenter,
		int xoyTiltLvl, float scaleFactor, m1::PackageStatus status)
	{
		std::vector<float> droneAABB = DroneAABB(droneCenter, xoyTiltLvl, status);

		float minXDrone{ droneAABB[0] }, maxXDrone{ droneAABB[1] };
		float minYDrone{ droneAABB[2] }, maxYDrone{ droneAABB[3] };
		float minZDrone{ droneAABB[4] }, maxZDrone{ droneAABB[5] };

		float closestX = std::max(minXDrone, std::min(cylinderCenter.x, maxXDrone));
		float closestZ = std::max(minZDrone, std::min(cylinderCenter.z, maxZDrone));

		float distance = glm::distance(glm::vec2(closestX, closestZ), glm::vec2(cylinderCenter.x, cylinderCenter.z));
		bool intersectsXOZ = (distance <= lit::treeTrunkRadius);

		float minYCylinder = cylinderCenter.y;
		float maxYCylinder = cylinderCenter.y + lit::treeTrunkHeight * scaleFactor;
		bool intersectsY = (minYDrone <= maxYCylinder && maxYDrone >= minYCylinder);

		return intersectsXOZ && intersectsY;
	}

	inline bool isDroneCollidingWithCones(glm::vec3 droneCenter, glm::vec3 conesCenter, int xoyTiltLvl, m1::PackageStatus status)
	{
		std::vector<float> droneAABB = DroneAABB(droneCenter, xoyTiltLvl, status);
		float minXDrone{ droneAABB[0] }, maxXDrone{ droneAABB[1] };
		float minYDrone{ droneAABB[2] }, maxYDrone{ droneAABB[3] };
		float minZDrone{ droneAABB[4] }, maxZDrone{ droneAABB[5] };

		float minYCones = conesCenter.y;
		float maxYCones = conesCenter.y + lit::treeCrownHeight;

		bool intersectsY = (minYDrone <= maxYCones && maxYDrone >= minYCones);

		if (!intersectsY) {
			return false;
		}

		float diffHeight = glm::clamp(maxYDrone, minYCones, maxYCones) - conesCenter.y;
		float currentRadius = lit::treeCrownRadius * (1.0f - (diffHeight / lit::treeCrownHeight));

		float closestX = std::max(minXDrone, std::min(conesCenter.x, maxXDrone));
		float closestZ = std::max(minZDrone, std::min(conesCenter.z, maxZDrone));

		float distance = glm::distance(glm::vec2(closestX, closestZ), glm::vec2(conesCenter.x, conesCenter.z));
		bool intersectsXOZ = (distance <= currentRadius);

		return intersectsXOZ && intersectsY;
	}

	inline bool isDroneCollidingWithField(glm::vec3 droneCenter, int xoyTiltLvl, float fieldSeed, m1::PackageStatus status)
	{
		std::vector<float> droneAABB = DroneAABB(droneCenter, xoyTiltLvl, status);
		float minXDrone = droneAABB[0], maxXDrone = droneAABB[1];
		float minYDrone = droneAABB[2];
		float minZDrone = droneAABB[4], maxZDrone = droneAABB[5];

		bool withinFieldXOZ = (minXDrone >= -lit::fieldX / 2.0f && maxXDrone <= lit::fieldX / 2.0f &&
			minZDrone >= -lit::fieldZ / 2.0f && maxZDrone <= lit::fieldZ / 2.0f);

		if (!withinFieldXOZ) {
			return true;
		}

		float y1{ fieldNoise(glm::vec2(minXDrone, minZDrone) * 5.0f, fieldSeed) };
		float y2{ fieldNoise(glm::vec2(minXDrone, maxZDrone) * 5.0f, fieldSeed) };
		
		float y3{ fieldNoise(glm::vec2(maxXDrone, minZDrone) * 5.0f, fieldSeed) };
		float y4{ fieldNoise(glm::vec2(maxXDrone, maxZDrone) * 5.0f, fieldSeed) };

		float center{ fieldNoise(glm::vec2(droneCenter.x, droneCenter.y) * 5.0f, fieldSeed) };
		float maxYField{ std::max(center, std::max(y1, std::max(y2, std::max(y3, y4))))};

		return minYDrone < maxYField;
	}

	inline bool isDroneInTheZone(glm::vec3 droneCenter, glm::vec3 deliveryZoneCenter)
	{
		float minXZone{ deliveryZoneCenter.x - lit::squareSide };
		float maxXZone{ deliveryZoneCenter.x + lit::squareSide };
		
		float minZZone{ deliveryZoneCenter.z - lit::squareSide };
		float maxZZone{ deliveryZoneCenter.z + lit::squareSide };

		float minZDrone{ droneCenter.z - (lit::droneBodyOX - lit::droneBodyOZ + lit::propellerOX) / 2.0f };
		float maxZDrone{ droneCenter.z + (lit::droneBodyOX - lit::droneBodyOZ + lit::propellerOX) / 2.0f };

		float minXDrone{ droneCenter.x - (lit::droneBodyOX - lit::droneBodyOZ + lit::propellerOX) / 2.0f };
		float maxXDrone{ droneCenter.x + (lit::droneBodyOX - lit::droneBodyOZ + lit::propellerOX) / 2.0f };

		return minXZone <= minXDrone && maxXDrone <= maxXZone &&
			minZZone <= minZDrone && maxZDrone <= maxZZone;
	}
}

#endif // !DRONE_H
