#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "drone_challenge.h"
#include "transforms3D.h"

#include "utils/glm_utils.h"

#include <vector>
#include <unordered_set>

namespace obstacle
{
	inline float RandomFloat(float min, float max)
	{
		return ((float)rand() / RAND_MAX) * (max - min) + min;
	}

	inline int RandomInt(int min, int max)
	{
		return min + (int)(((double)rand() / RAND_MAX) * (max - min + 1));
	}

	inline std::pair<glm::mat4, glm::mat4> GenerateTree(m1::Obstacle treeInfo)
	{
		glm::mat4 modelMatrixTrunk = transforms3D::Translate(treeInfo.position.x, 0.0f, treeInfo.position.y) *
			transforms3D::Scale(1.0f, treeInfo.scaleFactor, 1.0f);

		glm::mat4 modelMatrixCrown = transforms3D::Translate(treeInfo.position.x, lit::treeTrunkHeight * treeInfo.scaleFactor,
			treeInfo.position.y);

		return { modelMatrixTrunk, modelMatrixCrown };
	}

	inline glm::mat4 GenerateHouse(m1::Obstacle houseInfo)
	{
		return transforms3D::Translate(houseInfo.position.x, 0.0f, houseInfo.position.y) *
			transforms3D::Scale(1.0f, houseInfo.scaleFactor, 1.0f);
	}

	inline glm::mat4 GeneratePackage(m1::Obstacle packageInfo) {
		return transforms3D::Translate(packageInfo.position.x, 1.0f, packageInfo.position.y);
	}

	inline glm::mat4 GenerateZone(m1::Obstacle zoneInfo) {
		return transforms3D::Translate(zoneInfo.position.x, 1.0f, zoneInfo.position.y);
	}

	std::vector<m1::Obstacle> GeneratePositionsAndSizes(int numOfObstacles, std::vector<m1::Obstacle>& packagesAndZone);
}

#endif // !OBSTACLE_H
