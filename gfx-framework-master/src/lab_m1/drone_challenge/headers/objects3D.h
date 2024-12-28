#ifndef OBJECTS3D_H
#define OBJECTS3D_H

#include "literals.h"

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

#include <string>

namespace objects3D
{
	/* The field is made by a symmetric rectangle */
	Mesh* CreateField(const std::string& name, glm::vec3 startVertex);

	/* The tree trunk is made by a cylinder. */
	Mesh* CreateTreeTrunk(const std::string& name, glm::vec3 baseCenter, glm::vec3 color);

	/* The tree crown is made by 3 objects, two cone trunks and one cone. */
	Mesh* CreateTreeCrown(const std::string& name, glm::vec3 baseCenter, glm::vec3 color);

	/* The drone body is made by 2 parallelepipeds in a X form and 4 cubes on the margins */
	Mesh* CreateDroneBody(const std::string& name, glm::vec3 baseCenter, glm::vec3 color);

	/* The drone propellers are parallelepipeds on the cubes */
	Mesh* CreateDronePropeller(const std::string& name, glm::vec3 baseCenter, glm::vec3 color);

	/* The house body obstacle is made by a cube. */
	Mesh* CreateCube(const std::string& name, const float side, glm::vec3 color);

	/* The house roof is made by a prism. */
	Mesh* CreateHouseRoof(const std::string& name, glm::vec3 color);

	/* The delivery zone. */
	Mesh* CreateSquare(const std::string& name, glm::vec3 center, float length, glm::vec3 color, bool fill);

	/* The arrow path*/
	Mesh* CreateArrow(const std::string& name, glm::vec3 center, glm::vec3 color);

	/* Zone indicator */
	Mesh* CreateTriangle(const std::string& name, glm::vec3 center, glm::vec3 color);

	inline glm::vec3 RotateOY(glm::vec3 point, float angle) {
		return { glm::vec3(point.x * cos(angle) + point.z * sin(angle),
			point.y,
			-point.x * sin(angle) + point.z * cos(angle)) };
	}

	/* The vertices for the parallelepiped */
	inline std::vector<VertexFormat> CreateParallelepipedVertices(const float length, const float height,
		const float width, glm::vec3 center, glm::vec3 color) {

		std::vector<glm::vec3> vertices = {
			{center.x - length, center.y + 0.0f, center.z + width},
			{center.x + length, center.y + 0.0f, center.z + width},
			{center.x - length, center.y + height, center.z + width},
			{center.x + length, center.y + height, center.z + width},

			{center.x - length, center.y + 0.0f, center.z - width},
			{center.x + length, center.y + 0.0f, center.z - width},
			{center.x - length, center.y + height, center.z - width},
			{center.x + length, center.y + height, center.z - width},
		};

		std::vector<VertexFormat> rotatedVertices;
		for (const auto& vertex : vertices) {
			glm::vec3 rotatedPosition = RotateOY(vertex, lit::droneAngle);
			rotatedVertices.push_back({ rotatedPosition, color });
		}

		return rotatedVertices;
	}

	/* The indices for the parallelepiped */
	inline std::vector<unsigned int> CreateParallelepipedIndices(unsigned int index) {
		return {
			index, index + 1, index + 2,
			index + 1, index + 3, index + 2,
			index + 2, index + 3, index + 7,
			index + 2, index + 7, index + 6,
			index + 1, index + 7, index + 3,
			index + 1, index + 5, index + 7,
			index + 6, index + 7, index + 4,
			index + 7, index + 5, index + 4,
			index + 0, index + 4, index + 1,
			index + 1, index + 4, index + 5,
			index + 2, index + 6, index + 4,
			index + 0, index + 2, index + 4,
		};
	}
}

#endif // !OBJECTS3D_H
