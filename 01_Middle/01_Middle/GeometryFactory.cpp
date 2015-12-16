#include "GeometryFactory.h"

#include <vector>

#define M_PI    3.14159265358979323846264338327950288   /* pi */

// Static cube parameters.
const int GeometryFactory::N = 32;

const float GeometryFactory::knobHeight = 1.8f;
const float GeometryFactory::knobRadius = 2.4f;

const float GeometryFactory::cubeWidthUnit = 8.0f;
const float GeometryFactory::cubeWidthOffset = 0.1f;
const float GeometryFactory::thinCubeHeightUnit = 3.2f;
const float GeometryFactory::cubeTopWallThickness = 1.0f;
const float GeometryFactory::cubeSideWallThickness = 1.2f;

std::shared_ptr<Mesh> GeometryFactory::GetCuboid(glm::vec3 topLeftCorner, glm::vec3 bottomRightCorner)
{
	auto cuboid = std::make_shared<Mesh>();

	// calculate corner positions
	glm::vec3 a = topLeftCorner;
	glm::vec3 b = a; b.x = bottomRightCorner.x;
	glm::vec3 c = a; c.z = bottomRightCorner.z;
	glm::vec3 d = a; d.x = bottomRightCorner.x; d.z = bottomRightCorner.z;

	glm::vec3 h = bottomRightCorner;
	glm::vec3 e = h; e.x = topLeftCorner.x; e.z = topLeftCorner.z;
	glm::vec3 f = h; f.z = topLeftCorner.z;
	glm::vec3 g = h; g.x = topLeftCorner.x;

	// add sides
	glm::vec2 tex(0.0f, 0.0f);

	cuboid->addQuad(a, c, d, b, glm::vec3(0.0f, 1.0f, 0.0f), tex); // top
	cuboid->addQuad(c, g, h, d, glm::vec3(0.0f, 0.0f, 1.0f), tex); // front
	cuboid->addQuad(d, h, f, b, glm::vec3(1.0f, 0.0f, 0.0f), tex); // right
	cuboid->addQuad(b, f, e, a, glm::vec3(0.0f, 0.0f, -1.0f), tex); // back
	cuboid->addQuad(a, e, g, c, glm::vec3(-1.0f, 0.0f, 0.0f), tex); // left
	cuboid->addQuad(g, e, f, h, glm::vec3(0.0f, -1.0f, 0.0f), tex); // bottom

	return cuboid;
}

std::shared_ptr<Mesh> GeometryFactory::GetCircle(glm::vec3 center, float radius)
{
	auto circle = std::make_shared<Mesh>();

	for (int i = 0; i <= N; ++i)
	{
		float angle = (2 * M_PI / N) * i;
		glm::vec2 tex(0.25f * cos(angle + M_PI / 2.0f) + 0.5f, 0.25f * sin(angle + M_PI / 2.0f) + 0.5f);
		circle->addVertex({ glm::vec3(radius * cos(angle) + center.x, center.y, radius * sin(angle) + center.z), glm::vec3(0.0f, 1.0f, 0.0f), tex });
	}

	circle->addVertex({ center, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f) });

	for (int i = 0; i < N; ++i)
	{
		circle->addTriangleIndices(N + 1, i + 1, i);
	}

	return circle;
}

std::shared_ptr<Mesh> GeometryFactory::GetCylinder(glm::vec3 bottomCenterPosition, float radius, float height, bool reverse)
{
	auto cylinder = std::make_shared<Mesh>();

	std::vector<glm::vec3> topCorners, bottomCorners;

	for (int i = 0; i <= N; ++i)
	{
		float angle = (2 * M_PI / N) * i;
		topCorners.push_back(glm::vec3(radius * cos(angle) + bottomCenterPosition.x, height + bottomCenterPosition.y, radius * sin(angle) + bottomCenterPosition.z));
		bottomCorners.push_back(glm::vec3(radius * cos(angle) + bottomCenterPosition.x, bottomCenterPosition.y, radius * sin(angle) + bottomCenterPosition.z));
	}

	for (int i = 0; i < N; ++i)
	{
		float middleAngle = (2 * M_PI / N) * (2 * i + 1) / 2.0f;
		glm::vec3 normal(cos(middleAngle), 0.0f, sin(middleAngle));
		int currentIndex = i + 1, nextIndex = i;

		if (reverse)
		{
			currentIndex = i;
			nextIndex = i + 1;
		}

		cylinder->addQuad(topCorners[currentIndex], bottomCorners[currentIndex], bottomCorners[nextIndex], topCorners[nextIndex], normal, glm::vec2(0.0f, 0.0f));
	}

	return cylinder;
}

std::shared_ptr<Mesh> GeometryFactory::GetKnob(glm::vec3 bottomCenterPosition)
{
	auto knob = GetCylinder(bottomCenterPosition, knobRadius, knobHeight);
	knob->merge(GetCircle(bottomCenterPosition + glm::vec3(0.0f, knobHeight, 0.0f), knobRadius).get());

	return knob;
}

std::shared_ptr<Mesh> GeometryFactory::GetLegoCube(int rows, int cols, CubeHeight height)
{
	float xWidth = cols * cubeWidthUnit - 2 * cubeWidthOffset;
	float zWidth = rows * cubeWidthUnit - 2 * cubeWidthOffset;
	float yTop = (int)height * thinCubeHeightUnit;
	float yBottom = 0.0f;

	// assemble a lego cube's sides
	auto cubeMesh = GeometryFactory::GetCuboid(glm::vec3(cubeWidthOffset, yTop, cubeWidthOffset), glm::vec3(cubeWidthOffset + xWidth, yTop - cubeTopWallThickness, cubeWidthOffset + zWidth)); // top
	cubeMesh->merge(GeometryFactory::GetCuboid(glm::vec3(cubeWidthOffset, yTop, cubeWidthOffset + zWidth - cubeSideWallThickness), glm::vec3(cubeWidthOffset + xWidth, yBottom, cubeWidthOffset + zWidth)).get()); // front 
	cubeMesh->merge(GeometryFactory::GetCuboid(glm::vec3(cubeWidthOffset + xWidth - cubeSideWallThickness, yTop, cubeWidthOffset), glm::vec3(cubeWidthOffset + xWidth, yBottom, cubeWidthOffset + zWidth)).get()); // right
	cubeMesh->merge(GeometryFactory::GetCuboid(glm::vec3(cubeWidthOffset, yTop, cubeWidthOffset), glm::vec3(cubeWidthOffset + xWidth, yBottom, cubeWidthOffset + cubeSideWallThickness)).get()); // back
	cubeMesh->merge(GeometryFactory::GetCuboid(glm::vec3(cubeWidthOffset, yTop, cubeWidthOffset), glm::vec3(cubeWidthOffset + cubeSideWallThickness, yBottom, cubeWidthOffset + zWidth)).get()); // left

	// add knobs
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			float knobZ = r * cubeWidthUnit + (cubeWidthUnit / 2.0f);
			float knobX = c * cubeWidthUnit + (cubeWidthUnit / 2.0f);

			cubeMesh->merge(GetKnob(glm::vec3(knobX, yTop, knobZ)).get());
		}
	}

	return cubeMesh;
}
