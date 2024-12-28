#include "../headers/objects3D.h"
#include "../headers/literals.h"

Mesh* objects3D::CreateField(const std::string& name, glm::vec3 startVertex)
{
	std::vector<VertexFormat> vertices;

	/* Stores all positions in the intervals [-x, x] and [-z, z].
	The color and vertex normals are assigned in the VertexShader */
	for (float z = -lit::fieldZ / 2.0f; z <= lit::fieldZ / 2.0f; ++z) {
		for (float x = -lit::fieldX / 2.0f; x <= lit::fieldX / 2.0f; ++x) {
			glm::vec3 position = startVertex + glm::vec3(x, 0, z);
			vertices.push_back(VertexFormat(position));
		}
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < lit::fieldZ * (lit::fieldX + 1); ++i) {
		/* The vertices on the right and bottom edges are not
		the start of a rectangle of the field. */
		if ((i + 1) % (lit::fieldX + 1) == 0) {
			continue;
		}

		/* topLeft, bottomLeft, bottomRight */
		indices.insert(indices.end(), { i, i + lit::fieldX + 1, i + lit::fieldX + 2 });

		/* topRight, topLeft, bottomRight */
		indices.insert(indices.end(), { i + 1, i, i + lit::fieldX + 2 });
	}

	Mesh* field = new Mesh(name);
	field->InitFromData(vertices, indices);

	return field;
}

Mesh* objects3D::CreateTreeTrunk(const std::string& name, glm::vec3 baseCenter, glm::vec3 color)
{
	const unsigned int numPoints = lit::circlePoints;
	std::vector<VertexFormat> vertices;

	/* The two centers of the circles. */
	vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
	vertices.push_back(VertexFormat(glm::vec3(0, lit::treeTrunkHeight, 0), color));

	/* The vertices of the two circles */
	for (float h = 0.0f; h <= lit::treeTrunkHeight; h += lit::treeTrunkHeight) {
		for (unsigned int i = 0; i < numPoints; i++) {
			float angle = 2.0f * glm::pi<float>() * i / numPoints;
		
			glm::vec3 circleVertex(
				lit::treeTrunkRadius * cos(angle),
				h,
				lit::treeTrunkRadius * sin(angle));

			vertices.push_back(VertexFormat(circleVertex, color));
		}
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < numPoints; i++) {
		/* bottom circle */
		indices.insert(indices.end(), { 0, 2 + i, 2 + ((i + 1) % numPoints) });
		
		/* top circle */
		indices.insert(indices.end(), { 1, 2 + numPoints + ((i + 1) % numPoints), 2 + numPoints + i });

		/* the triangles connecting the bases */
		/* topLeft, bottomLeft, bottomRight */
		indices.insert(indices.end(), { 2 + numPoints + i, 2 + i, 2 + ((i + 1) % numPoints) });

		/* topRight, topLeft, bottomRight */
		indices.insert(indices.end(), { 2 + numPoints + ((i + 1) % numPoints), 2 + numPoints + i, 2 + ((i + 1) % numPoints) });
	}

	Mesh* treeTrunk = new Mesh(name);
	treeTrunk->InitFromData(vertices, indices);

	return treeTrunk;
}

Mesh* objects3D::CreateTreeCrown(const std::string& name, glm::vec3 baseCenter, glm::vec3 color)
{
	const unsigned int numPoints = lit::circlePoints;

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	float currentHeight = 0.0f;

	/* the first two trunk cones and after them the cone */
	for (int obj = 0; obj < 3; ++obj) {
		unsigned int centerIndex = (unsigned int) vertices.size();

		/* circle vertices */
		for (unsigned int i = 0; i < numPoints; ++i) {
			float angle = 2.0f * glm::pi<float>() * i / numPoints;

			glm::vec3 center = baseCenter + glm::vec3(
				lit::treeCrownRadius * cos(angle),
				currentHeight,
				lit::treeCrownRadius * sin(angle));

			vertices.push_back(VertexFormat(center, color));
		}

		glm::vec3 apex = baseCenter + glm::vec3(0.0f, currentHeight + lit::treeCrownHeight / 1.6f, 0.0f);
		
		/* the apex */
		unsigned int apexIndex = (unsigned int) vertices.size();
		vertices.push_back(VertexFormat(apex, color));

		for (unsigned int i = 0; i < numPoints; ++i) {
			indices.insert(indices.end(), { centerIndex + i, apexIndex, centerIndex + (i + 1) % numPoints});
		}

		currentHeight += lit::treeCrownHeight / 5.33f;
	}

	Mesh* treeCrown = new Mesh(name);
	treeCrown->InitFromData(vertices, indices);

	return treeCrown;
}

Mesh* objects3D::CreateDroneBody(const std::string& name, glm::vec3 baseCenter, glm::vec3 color)
{
	std::vector<VertexFormat> vertices;

	/* The two parallelepipeds vertices */
	auto firstParallVert = CreateParallelepipedVertices(lit::droneBodyOX / 2.0f, lit::droneBodyOY, lit::droneBodyOZ / 2.0f, baseCenter, color);
	vertices.insert(vertices.end(), firstParallVert.begin(), firstParallVert.end());

	auto secondParallVert = CreateParallelepipedVertices(lit::droneBodyOZ / 2.0f, lit::droneBodyOY, lit::droneBodyOX / 2.0f, baseCenter, color);
	vertices.insert(vertices.end(), secondParallVert.begin(), secondParallVert.end());

	/* The four cubes vertices */
	glm::vec3 lowerCubeCenter = baseCenter + glm::vec3(0.0f, lit::droneBodyOY, lit::droneBodyOX / 2.0f - lit::droneBodyOZ / 2.0f);
	auto lowerCubeVert = CreateParallelepipedVertices(lit::droneBodyOZ / 2.0f, lit::droneBodyOZ, lit::droneBodyOZ / 2.0f, lowerCubeCenter, color);
	vertices.insert(vertices.end(), lowerCubeVert.begin(), lowerCubeVert.end());

	glm::vec3 leftCubeCenter = baseCenter + glm::vec3(-lit::droneBodyOX / 2.0f + lit::droneBodyOZ / 2.0f, lit::droneBodyOY, 0.0f);
	auto leftCubeVert = CreateParallelepipedVertices(lit::droneBodyOZ / 2.0f, lit::droneBodyOZ, lit::droneBodyOZ / 2.0f, leftCubeCenter, color);
	vertices.insert(vertices.end(), leftCubeVert.begin(), leftCubeVert.end());

	glm::vec3 upperCubeCenter = baseCenter + glm::vec3(0.0f, lit::droneBodyOY, -lit::droneBodyOX / 2.0f + lit::droneBodyOZ / 2.0f);
	auto upperCubeVert = CreateParallelepipedVertices(lit::droneBodyOZ / 2.0f, lit::droneBodyOZ, lit::droneBodyOZ / 2.0f, upperCubeCenter, color);
	vertices.insert(vertices.end(), upperCubeVert.begin(), upperCubeVert.end());

	glm::vec3 rightCubeCenter = baseCenter + glm::vec3(lit::droneBodyOX / 2.0f - lit::droneBodyOZ / 2.0f, lit::droneBodyOY, 0.0f);
	auto rightCubeVert = CreateParallelepipedVertices(lit::droneBodyOZ / 2.0f, lit::droneBodyOZ, lit::droneBodyOZ / 2.0f, rightCubeCenter, color);
	vertices.insert(vertices.end(), rightCubeVert.begin(), rightCubeVert.end());

	std::vector<unsigned int> indices;

	/* The two parallelepipeds indices */
	auto firstParallInd = CreateParallelepipedIndices(0);
	indices.insert(indices.end(), firstParallInd.begin(), firstParallInd.end());

	auto secondParallInd = CreateParallelepipedIndices(8);
	indices.insert(indices.end(), secondParallInd.begin(), secondParallInd.end());

	/* The four cubes indices */
	auto lowerCubeInd = CreateParallelepipedIndices(16);
	indices.insert(indices.end(), lowerCubeInd.begin(), lowerCubeInd.end());

	auto leftCubeInd = CreateParallelepipedIndices(24);
	indices.insert(indices.end(), leftCubeInd.begin(), leftCubeInd.end());

	auto upperCubeInd = CreateParallelepipedIndices(32);
	indices.insert(indices.end(), upperCubeInd.begin(), upperCubeInd.end());

	auto rightCubeInd = CreateParallelepipedIndices(40);
	indices.insert(indices.end(), rightCubeInd.begin(), rightCubeInd.end());

	Mesh* body = new Mesh(name);
	body->InitFromData(vertices, indices);

	return body;
}

Mesh* objects3D::CreateDronePropeller(const std::string& name, glm::vec3 baseCenter, glm::vec3 color)
{
	std::vector<VertexFormat> vertices;

	/* The propellers vertices */
	auto propellerVert = CreateParallelepipedVertices(lit::propellerOX / 2.0f, lit::propellerOY, lit::propellerOZ / 2.0f, baseCenter, color);
	vertices.insert(vertices.end(), propellerVert.begin(), propellerVert.end());

	std::vector<unsigned int> indices;

	/* The propeller indices */
	auto propellerInd = CreateParallelepipedIndices(0);
	indices.insert(indices.end(), propellerInd.begin(), propellerInd.end());
	
	Mesh* propeller = new Mesh(name);
	propeller->InitFromData(vertices, indices);

	return propeller;
}

Mesh* objects3D::CreateCube(const std::string& name, const float side, glm::vec3 color)
{
	std::vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(-side / 2.0f, 0, side / 2.0f), color),
		VertexFormat(glm::vec3(side / 2.0f, 0, side / 2.0f), color),
		VertexFormat(glm::vec3(-side / 2.0f, side, side / 2.0f), color),
		VertexFormat(glm::vec3(side / 2.0f, side, side / 2.0f), color),
		VertexFormat(glm::vec3(-side / 2.0f, 0, -side / 2.0f), color),
		VertexFormat(glm::vec3(side / 2.0f, 0, -side / 2.0f), color),
		VertexFormat(glm::vec3(-side / 2.0f, side, -side / 2.0f), color),
		VertexFormat(glm::vec3(side / 2.0f, side, -side / 2.0f), color),
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		1, 3, 2,
		2, 3, 7,
		2, 7, 6,
		1, 7, 3,
		1, 5, 7,
		6, 7, 4,
		7, 5, 4,
		0, 4, 1,
		1, 4, 5,
		2, 6, 4,
		0, 2, 4,
	};

	Mesh* houseBody = new Mesh(name);
	houseBody->InitFromData(vertices, indices);

	return houseBody;
}

Mesh* objects3D::CreateHouseRoof(const std::string& name, glm::vec3 color)
{
	const float side = lit::houseSide;
	const float height = lit::roofHeight;

	std::vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(-side / 2.0f, side, side / 2.0f), color),
		VertexFormat(glm::vec3(side / 2.0f, side, side / 2.0f), color),
		VertexFormat(glm::vec3(-side / 2.0f, side, -side / 2.0f), color),
		VertexFormat(glm::vec3(side / 2.0f, side, -side / 2.0f), color),
		VertexFormat(glm::vec3(0, side + height, 0), color) 
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 4,
		1, 3, 4,
		3, 2, 4,
		2, 0, 4 
	};

	Mesh* roof = new Mesh(name);
	roof->InitFromData(vertices, indices);
	return roof;
}

Mesh* objects3D::CreateSquare(const std::string& name, glm::vec3 center, float length, glm::vec3 color, bool fill)
{
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(-length / 2.0f, 0.0f, -length / 2.0f), color),
		VertexFormat(center + glm::vec3(-length / 2.0f, 0.0f, length / 2.0f), color),
		VertexFormat(center + glm::vec3(length / 2.0f, 0.0f, length / 2.0f), color),
		VertexFormat(center + glm::vec3(length / 2.0f, 0.0f, -length / 2.0f), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* objects3D::CreateArrow(const std::string& name, glm::vec3 center, glm::vec3 color)
{
	std::vector<VertexFormat> vertices = {
		VertexFormat(center + glm::vec3(0.0f, 0.0f, -1.0f), color),
		VertexFormat(center + glm::vec3(-0.5f, 0.0f, 0.0f), color),
		VertexFormat(center + glm::vec3(0.5f, 0.0f, 0.0f), color),

		VertexFormat(center + glm::vec3(-0.2f, 0.0f, 0.0f), color),
		VertexFormat(center + glm::vec3(0.2f, 0.0f, 0.0f), color),
		VertexFormat(center + glm::vec3(-0.2f, 0.0f, 1.0f), color),
		VertexFormat(center + glm::vec3(0.2f, 0.0f, 1.0f), color),
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		3, 4, 5,
		4, 6, 5
	};

	Mesh* arrow = new Mesh(name);
	arrow->InitFromData(vertices, indices);

	return arrow;
}

Mesh* objects3D::CreateTriangle(const std::string& name, glm::vec3 center, glm::vec3 color)
{
	std::vector<VertexFormat> vertices = {
		VertexFormat(center + glm::vec3(0.0f, -lit::indicatorHeight / 2.0f, 0.0f), color),
		VertexFormat(center + glm::vec3(-lit::indicatorBase / 2.0f, lit::indicatorHeight / 2.0f, 0.0f), color),
		VertexFormat(center + glm::vec3(lit::indicatorBase / 2.0f, lit::indicatorHeight / 2.0f, 0.0f), color)
	};

	std::vector<unsigned int> indices = {
		0, 1, 2 
	};

	Mesh* triangle = new Mesh(name);
	triangle->InitFromData(vertices, indices);

	return triangle;
}
