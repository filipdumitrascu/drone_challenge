#ifndef LITERALS_H
#define LITERALS_H

#include "utils/glm_utils.h"

namespace lit
{
	// dimensions
	constexpr int circlePoints{ 30 };
	constexpr int numOfObstacles{ 35 };

	constexpr int numOfPackages{ 3 };
	constexpr int numOfZones{ numOfPackages };

	constexpr int fieldX{ 50 };
	constexpr int fieldZ{ 50 };

	constexpr float droneBodyOX{ fieldX / 30.0f };
	constexpr float droneBodyOY{ 0.15f };
	constexpr float droneBodyOZ{ fieldZ / 400.0f };

	constexpr float propellerOX{ fieldX / 100.0f };
	constexpr float propellerOY{ 0.05f };
	constexpr float propellerOZ{ fieldZ / 850.0f };

	constexpr float treeTrunkHeight{ 2.0f };
	constexpr float treeTrunkRadius{ fieldX / 125.0f };

	constexpr float treeCrownHeight{ 3.0f };
	constexpr float treeCrownRadius{ fieldX / 45.0f };

	constexpr float roofHeight{ 1.25f };
	constexpr float houseSide{ fieldX / 25.0f };

	constexpr float packageSide{ fieldX / 100.0f };
	constexpr float squareSide{ 4.0f * packageSide };

	constexpr float maxObsHeight{ lit::treeTrunkHeight * 1.5f + lit::treeCrownHeight };

	constexpr float droneAngle{ RADIANS(45.0f) };
	constexpr float sphereRadius{ (droneBodyOX + propellerOX - droneBodyOZ) / 2.0f };

	constexpr float fov{ RADIANS(60.0f) };
	constexpr float near{ 0.01f };
	constexpr float far{ 200.0f };

	constexpr float zNear{ 0.1f };
	constexpr float zFar{ 100.0f };

	constexpr float arrowHeight{ 2.5f };

	constexpr float indicatorHeight{ 1.5f };
	constexpr float indicatorBase{ 1.0f };

	// colors
	constexpr glm::vec3 origin{ glm::vec3(0.0f, 0.0f, 0.0f) };
	constexpr glm::vec3 green{ glm::vec3(0.0f, 0.75f, 0.0f) };

	constexpr glm::vec3 darkBrown{ glm::vec3(0.4f, 0.2f, 0.1f) };
	constexpr glm::vec3 lightBrown{ glm::vec3(0.7f, 0.5f, 0.3f) };

	constexpr glm::vec3 cream{ glm::vec3(1.0f, 1.0f, 0.8f) };
	constexpr glm::vec3 scarletRed{ glm::vec3(0.8f, 0.2f, 0.2f) };

	constexpr glm::vec3 gray{ glm::vec3(0.8f, 0.8f, 0.8f) };
	constexpr glm::vec3 black{ glm::vec3(0.0f, 0.0f, 0.0f) };

	// backgroud
	constexpr float backgroundRed{ 0.678f };
	constexpr float backgroundGreen{ 0.847f };
	constexpr float backgroundBlue{ 0.902f };
}

#endif // !LITERALS_H
