#include "../headers/literals.h"
#include "../headers/obstacles.h"

std::vector<m1::Obstacle> obstacle::GeneratePositionsAndSizes(int numOfObstacles,
    std::vector<m1::Obstacle>& packagesAndZone)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    std::vector<m1::Obstacle> obstacles;

    /* Radius of the area occupied by the largest obstacle  */
    const float maxObstacleRadius = lit::houseSide * sqrtf(2.0f) / 2.0f;

    /* The radius of the surface where an obstacle can be placed so that collisions with other obstacles do not occur */
	const float zoneRadius = std::min(lit::fieldX / (2.0f * std::sqrtf((float) numOfObstacles)),
        lit::fieldZ / (2.0f * std::sqrtf((float) numOfObstacles)));

    /* How many such areas are on ox and oy. */
	int zonesX = static_cast<int>(lit::fieldX / (2 * zoneRadius));
	int zonesZ = static_cast<int>(lit::fieldZ / (2 * zoneRadius));

    /* Uniform the field. */
    while (zonesX * zonesZ < numOfObstacles) {
        zonesX < zonesZ ? ++zonesX : ++zonesZ;
    }

    /* Choose 4 random positions for the packages and delivery zone. */
    std::unordered_set<int> deliveryIndexes;
    while (deliveryIndexes.size() < (size_t) (lit::numOfPackages + lit::numOfZones)) {
        deliveryIndexes.insert(RandomInt(0, numOfObstacles - 1));
    }
    std::vector<int> delivery(deliveryIndexes.begin(), deliveryIndexes.end());

    for (int i = 0; i < numOfObstacles; ++i) {
        int zoneX = i % zonesX;
        int zoneZ = i / zonesZ;

        float centerX = -lit::fieldX / 2.0f + zoneRadius + 2.0f * zoneRadius * zoneX;
        float centerZ = -lit::fieldZ / 2.0f + zoneRadius + 2.0f * zoneRadius * zoneZ;

        /* Calculates a random position starting from zone center. */
        float randomOffsetX = RandomFloat(-zoneRadius + maxObstacleRadius, zoneRadius - maxObstacleRadius);
        float randomOffsetZ = RandomFloat(-zoneRadius + maxObstacleRadius, zoneRadius - maxObstacleRadius);

        float scaleFactorTree = RandomFloat(0.5f, 1.5f);
        float scaleFactorHouse = RandomFloat(0.85f, 1.35f);
        float obstacleType = RandomFloat(0.0f, 1.0f);

        m1::ObstacleType obsType = obstacleType <= 0.75f ? m1::ObstacleType::TREE : m1::ObstacleType::HOUSE;
        float scaleFactor = obsType == m1::ObstacleType::HOUSE ? scaleFactorHouse : scaleFactorTree;

        m1::ObstacleType delivType{};
        
        bool pushed{ false };

        for (int j = 0; j < (lit::numOfPackages + lit::numOfZones) / 2; ++j) {
            if (i == delivery[j]) {
                packagesAndZone.push_back({ glm::vec2(centerX + randomOffsetX, centerZ + randomOffsetZ), 1.0f, m1::ObstacleType::ZONE });
                pushed = true;
                break;
            }
        }

        if (pushed) {
            continue;
        }

        for (int j = (lit::numOfPackages + lit::numOfZones) / 2; j < lit::numOfPackages + lit::numOfZones; ++j) {
            if (i == delivery[j]) {
                packagesAndZone.push_back({ glm::vec2(centerX + randomOffsetX, centerZ + randomOffsetZ), 1.0f, m1::ObstacleType::PACKAGE });
                pushed = true;
                break;
            }
        }
        
        if (pushed) {
            continue;
        }

        obstacles.push_back({ glm::vec2(centerX + randomOffsetX, centerZ + randomOffsetZ), scaleFactor, obsType });
    }

	return obstacles;
}
