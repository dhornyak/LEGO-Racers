#include "GeometryFactory.h"

#include "TrackSection.h"

#include <vector>

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

std::shared_ptr<Mesh> GeometryFactory::GetCircle(glm::vec3 center, glm::vec3 normal, float radius)
{
	auto circle = std::make_shared<Mesh>();

	for (int i = 0; i <= N; ++i)
	{
		float angle = (2 * M_PI / N) * i;
		glm::vec2 tex(0.25f * cos(angle + M_PI / 2.0f) + 0.5f, 0.25f * sin(angle + M_PI / 2.0f) + 0.5f);

		if (normal.x == 1.0f)
		{
			circle->addVertex({ glm::vec3(0.0f, radius * sin(angle), radius * cos(angle)) + center, normal, tex });
		}
		else if (normal.x == -1.0f)
		{
			circle->addVertex({ glm::vec3(0.0f, radius * cos(angle), radius * sin(angle)) + center, normal, tex });
		}
		else if (normal.y == 1.0f)
		{
			circle->addVertex({ glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle)) + center, normal, tex });
		}
		else if (normal.y == -1.0f)
		{
			circle->addVertex({ glm::vec3(radius * sin(angle), 0.0f, radius * cos(angle)) + center, normal, tex });
		}
		else if (normal.z == 1.0f)
		{
			circle->addVertex({ glm::vec3(radius * sin(angle), radius * cos(angle), 0.0f) + center, normal, tex });
		}
		else if (normal.z == -1.0f)
		{
			circle->addVertex({ glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f) + center, normal, tex });
		}
	}

	circle->addVertex({ center, normal, glm::vec2(0.5f, 0.5f) });

	for (int i = 0; i < N; ++i)
	{
		circle->addTriangleIndices(N + 1, i + 1, i);
	}

	return circle;
}

std::shared_ptr<Mesh> GeometryFactory::GetCylinder(glm::vec3 bottomCenterPosition, glm::vec3 direction, float radius, float height, bool reverse)
{
	auto cylinder = std::make_shared<Mesh>();

	std::vector<glm::vec3> topCorners, bottomCorners;

	for (int i = 0; i <= N; ++i)
	{
		float angle = (2 * M_PI / N) * i;

		if (direction.x == 1.0f)
		{
			topCorners.push_back(glm::vec3(height, radius * sin(angle), radius * cos(angle)) + bottomCenterPosition);
			bottomCorners.push_back(glm::vec3(0.0f, radius * sin(angle), radius * cos(angle)) + bottomCenterPosition);
		}
		else if (direction.y == 1.0f)
		{
			topCorners.push_back(glm::vec3(radius * cos(angle), height, radius * sin(angle)) + bottomCenterPosition);
			bottomCorners.push_back(glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle)) + bottomCenterPosition);
		}
		else if (direction.z == 1.0f)
		{
			topCorners.push_back(glm::vec3(radius * sin(angle), radius * cos(angle), height) + bottomCenterPosition);
			bottomCorners.push_back(glm::vec3(radius * sin(angle), radius * cos(angle), 0.0f) + bottomCenterPosition);
		}
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

glm::vec3 GeometryFactory::CalculateSphereCoordinate(float x, float y, float radius)
{
	float u = 2 * M_PI * x;
	float v = M_PI * y;

	glm::vec3 vertex(radius * cos(u)*sin(v), radius * cos(v), radius * sin(u)*sin(v));
	return vertex;
}

std::shared_ptr<Mesh> GeometryFactory::GetSphere(glm::vec3 center, float radius)
{
	auto sphere = std::make_shared<Mesh>();

	float delta = 1.0 / N;
	glm::vec2 tex(0.0f, 0.0f);

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			float x = i * delta;
			float y = j * delta;

			// 1. háromszög: x,y x+delta,y y+delta,x
			auto a = CalculateSphereCoordinate(x, y, radius) + center;
			auto b = CalculateSphereCoordinate(x + delta, y, radius) + center;
			auto c = CalculateSphereCoordinate(x, y + delta, radius) + center;

			auto dir = glm::cross((b - a), (c - a)); // cross product
			sphere->addTriangle(a, b, c, dir / (float)dir.length(), tex);

			// 2. háromszög: x+delta,y x+delta,y+delta y+delta,x
			a = CalculateSphereCoordinate(x + delta, y, radius) + center;
			b = CalculateSphereCoordinate(x + delta, y + delta, radius) + center;
			c = CalculateSphereCoordinate(x, y + delta, radius) + center;

			dir = glm::cross((b - a), (c - a)); // cross product
			sphere->addTriangle(a, b, c, dir / (float)dir.length(), tex);
		}
	}

	return sphere;
}

std::shared_ptr<Mesh> GeometryFactory::GetKnob(glm::vec3 bottomCenterPosition)
{
	auto knob = GetCylinder(bottomCenterPosition, glm::vec3(0.0f, 1.0f, 0.0f), knobRadius, knobHeight);
	knob->merge(GetCircle(bottomCenterPosition + glm::vec3(0.0f, knobHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), knobRadius).get());

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

std::shared_ptr<Mesh> GeometryFactory::GetDriverTorso(glm::vec3 bottomCenterPosition)
{
	auto torso = std::make_shared<Mesh>();

	float height = 5.0f * thinCubeHeightUnit;

	float bottomXWidthHalf = 1.0f * cubeWidthUnit - cubeWidthOffset;
	float bottomZWidthHalf = 0.5f * cubeWidthUnit - cubeWidthOffset;

	float topXWidthHalf = 0.75f * cubeWidthUnit - cubeWidthOffset;
	float topZWidthHalf = 0.5f * cubeWidthUnit - cubeWidthOffset;

	glm::vec3 topCenterPosition = bottomCenterPosition;
	topCenterPosition.y += height;

	// corner positions
	glm::vec3 bottomE(bottomCenterPosition.x - bottomXWidthHalf, bottomCenterPosition.y, bottomCenterPosition.z - bottomZWidthHalf);
	glm::vec3 bottomF(bottomCenterPosition.x + bottomXWidthHalf, bottomCenterPosition.y, bottomCenterPosition.z - bottomZWidthHalf);
	glm::vec3 bottomG(bottomCenterPosition.x - bottomXWidthHalf, bottomCenterPosition.y, bottomCenterPosition.z + bottomZWidthHalf);
	glm::vec3 bottomH(bottomCenterPosition.x + bottomXWidthHalf, bottomCenterPosition.y, bottomCenterPosition.z + bottomZWidthHalf);

	glm::vec3 topA(topCenterPosition.x - topXWidthHalf, topCenterPosition.y, topCenterPosition.z - topZWidthHalf);
	glm::vec3 topB(topCenterPosition.x + topXWidthHalf, topCenterPosition.y, topCenterPosition.z - topZWidthHalf);
	glm::vec3 topC(topCenterPosition.x - topXWidthHalf, topCenterPosition.y, topCenterPosition.z + topZWidthHalf);
	glm::vec3 topD(topCenterPosition.x + topXWidthHalf, topCenterPosition.y, topCenterPosition.z + topZWidthHalf);

	glm::vec2 tex(0.0f, 0.0f);

	torso->addQuad(topA, topC, topD, topB, glm::vec3(0.0f, 1.0f, 0.0f), tex); // top
	torso->addQuad(topC, bottomG, bottomH, topD, glm::vec3(0.0f, 0.0f, 1.0f), tex); // front
	torso->addQuad(topD, bottomH, bottomF, topB, glm::vec3(1.0f, 0.0f, 0.0f), tex); // right
	torso->addQuad(topB, bottomF, bottomE, topA, glm::vec3(0.0f, 0.0f, -1.0f), tex); // back
	torso->addQuad(topA, bottomE, bottomG, topC, glm::vec3(-1.0f, 0.0f, 0.0f), tex); // left
	torso->addQuad(bottomG, bottomE, bottomF, bottomH, glm::vec3(0.0f, -1.0f, 0.0f), tex); // bottom

	return torso;
}

std::shared_ptr<Mesh> GeometryFactory::GetDriver()
{
	auto driver = std::make_shared<Mesh>();

	float xWidth = 1 * cubeWidthUnit - 2 * cubeWidthOffset;
	float zWidth = 2 * cubeWidthUnit - 2 * cubeWidthOffset;

	// left leg
	driver = GetCuboid(
		glm::vec3(cubeWidthOffset, 2.0 * thinCubeHeightUnit, cubeWidthOffset),
		glm::vec3(cubeWidthOffset + xWidth, 0.0f, cubeWidthOffset + zWidth));

	driver->merge(GetCuboid(
		glm::vec3(cubeWidthOffset, 3.0 * thinCubeHeightUnit, cubeWidthOffset + zWidth - thinCubeHeightUnit),
		glm::vec3(cubeWidthOffset + xWidth, 0.0f, cubeWidthOffset + zWidth)).get());

	// right leg
	driver->merge(GetCuboid(
		glm::vec3(2.0f * cubeWidthOffset + xWidth, 2.0 * thinCubeHeightUnit, cubeWidthOffset),
		glm::vec3(2.0f * cubeWidthOffset + 2.0f * xWidth, 0.0f, cubeWidthOffset + zWidth)).get());

	driver->merge(GetCuboid(
		glm::vec3(2.0f * cubeWidthOffset + xWidth, 3.0 * thinCubeHeightUnit, cubeWidthOffset + zWidth - thinCubeHeightUnit),
		glm::vec3(2.0f * cubeWidthOffset + 2.0f * xWidth, 0.0f, cubeWidthOffset + zWidth)).get());

	// torso
	driver->merge(GetDriverTorso(glm::vec3(cubeWidthOffset + xWidth, 2.0 * thinCubeHeightUnit + cubeWidthOffset, cubeWidthOffset + 0.5 * cubeWidthUnit)).get());

	// left arm
	driver->merge(GetCuboid(
		glm::vec3(cubeWidthOffset, 5.5 * thinCubeHeightUnit, cubeWidthOffset),
		glm::vec3(cubeWidthOffset + xWidth / 2.0f, 4.5f * thinCubeHeightUnit, cubeWidthOffset + zWidth)).get());

	// right arm
	driver->merge(GetCuboid(
		glm::vec3(2.0f * cubeWidthOffset + 1.5f * xWidth, 5.5 * thinCubeHeightUnit, cubeWidthOffset),
		glm::vec3(2.0f * cubeWidthOffset + 2.0f * xWidth, 4.5f * thinCubeHeightUnit, cubeWidthOffset + zWidth)).get());

	// head
	driver->merge(GeometryFactory::GetSphere(glm::vec3(cubeWidthOffset + xWidth, 8.5 * thinCubeHeightUnit - cubeWidthOffset, cubeWidthOffset + 0.5 * cubeWidthUnit), cubeWidthUnit / 1.5 - cubeWidthOffset).get());

	// driving wheel base
	driver->merge(GetCuboid(
		glm::vec3(cubeWidthOffset, thinCubeHeightUnit, 2 * cubeWidthOffset + zWidth),
		glm::vec3(2.0f * cubeWidthOffset + 2.0f * xWidth, 0.0f, 2 * cubeWidthOffset + zWidth + cubeWidthUnit)).get());

	// driving wheel cylinder
	driver->merge(GetCylinder(glm::vec3(cubeWidthOffset + xWidth, 0.0f, 2.0f * cubeWidthOffset + 1.25f * zWidth), glm::vec3(0.0f, 1.0f, 0.0f), knobRadius / 2.0f, 6.0 * thinCubeHeightUnit).get());

	// driving wheel circle
	driver->merge(GetCircle(glm::vec3(cubeWidthOffset + xWidth, 6.0 * thinCubeHeightUnit, 2.0f * cubeWidthOffset + 1.25f * zWidth), glm::vec3(0.0f, 1.0f, 0.0f), knobRadius / 2.0f).get());

	// driving wheel
	driver->merge(GetCuboid(
		glm::vec3(cubeWidthOffset + xWidth / 2.0f, 7.0f * thinCubeHeightUnit, 2.0f * cubeWidthOffset + 1.20f * zWidth),
		glm::vec3(2.0f * cubeWidthOffset + 1.5f * xWidth, 5.0f * thinCubeHeightUnit, 2.0f * cubeWidthOffset + 1.30f * zWidth)).get());

	return driver;
}

std::shared_ptr<Mesh> GeometryFactory::GetReflector()
{
	auto reflector = GetLegoCube(1, 1, CubeHeight::THIN);
	reflector->merge(GetCylinder(glm::vec3(cubeWidthUnit / 2.0f, 0.0f, cubeWidthUnit / 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), knobHeight / 2.0f, 3.0f * thinCubeHeightUnit).get());
	reflector->merge(GetCuboid(
		glm::vec3(cubeWidthOffset, 6.0f * thinCubeHeightUnit, cubeWidthOffset),
		glm::vec3(cubeWidthOffset + cubeWidthUnit, 3.0f * thinCubeHeightUnit, cubeWidthOffset + cubeWidthUnit)).get());
	reflector->merge(GetKnob(glm::vec3(cubeWidthUnit / 2.0f, 6.0f * thinCubeHeightUnit, cubeWidthUnit / 2.0f)).get());
	reflector->merge(GetSphere(glm::vec3(cubeWidthUnit / 2.0f, 4.5f * thinCubeHeightUnit, cubeWidthUnit), 1.4f * knobRadius).get());

	return reflector;
}

std::shared_ptr<Mesh> GeometryFactory::GetWheel()
{
	// Outer rim.
	auto wheel = GetCylinder(
		glm::vec3(0.0f, 1.5f * cubeWidthUnit, 1.5f * cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		1.5f * cubeWidthUnit, cubeWidthUnit);
	wheel->merge(GetCircle(
		glm::vec3(0.0f, 1.5f * cubeWidthUnit, 1.5f * cubeWidthUnit),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		1.5f * cubeWidthUnit).get());
	wheel->merge(GetCircle(
		glm::vec3(cubeWidthUnit, 1.5f * cubeWidthUnit, 1.5f * cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		1.5f * cubeWidthUnit).get());

	// Inner rim.
	wheel->merge(GetCylinder(
		glm::vec3(cubeWidthUnit, 1.5f * cubeWidthUnit, 1.5f * cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		1.0f * cubeWidthUnit, 0.2f * cubeWidthUnit).get());
	wheel->merge(GetCircle(
		glm::vec3(1.2f * cubeWidthUnit, 1.5f * cubeWidthUnit, 1.5f * cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		1.0f * cubeWidthUnit).get());

	return wheel;
}

std::shared_ptr<Mesh> GeometryFactory::GetLineTrackMesh(std::shared_ptr<const Line> line, float halfWidth)
{
	auto lineMesh = std::make_shared<Mesh>();
	int quadNum = 16;

	for (int i = 0; i < 16; ++i)
	{
		switch (line->orientation)
		{
		case Line::Orientation::HORIZONTAL:
		{
			float width = abs(line->start.x - line->end.x);
			float startX = (line->start.x < line->end.x) ? 0.0f : -width;

			float currentX = (width / quadNum) * i + startX;
			float nextX = (width / quadNum) * (i + 1) + startX;
			float Y = line->start.y;
			// float zTop = line->start.z - halfWidth, zBottom = line->start.z + halfWidth;
			float zTop = 0.0f - halfWidth, zBottom = 0.0f + halfWidth;

			glm::vec3 a(currentX, Y, zTop);
			glm::vec3 b(currentX, Y, zBottom);
			glm::vec3 c(nextX, Y, zBottom);
			glm::vec3 d(nextX, Y, zTop);

			lineMesh->addQuadTex(a, b, c, d, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		break;
		case Line::Orientation::VERTICAL:
		{
			float height = abs(line->start.z - line->end.z);
			float startZ = (line->start.z < line->end.z) ? 0.0f : -height;

			float currentZ = (height / quadNum) * i + startZ;
			float nextZ = (height / quadNum) * (i + 1) + startZ;
			float Y = line->start.y;
			// float xLeft = line->start.x - halfWidth, xRight = line->start.x + halfWidth;
			float xLeft = 0.0f - halfWidth, xRight = 0.0f + halfWidth;

			glm::vec3 a(xLeft, Y, currentZ);
			glm::vec3 b(xLeft, Y, nextZ);
			glm::vec3 c(xRight, Y, nextZ);
			glm::vec3 d(xRight, Y, currentZ);

			lineMesh->addQuadTex(a, b, c, d, glm::vec3(0.0f, 1.0f, 0.0f));
		}
			break;
		default:
			break;
		}
	}

	return lineMesh;
}

std::shared_ptr<Mesh> GeometryFactory::GetCornerTrackMesh(std::shared_ptr<const Corner> corner, float halfWidth)
{
	auto cornerMesh = std::make_shared<Mesh>();
	float Y = corner->center.y;

	/*glm::vec3 a(0.0f, Y, -2.0 * corner->radius);
	glm::vec3 b(0.0f, Y, -0.5f * corner->radius);
	glm::vec3 c(0.5f * corner->radius, Y, 0.0f);
	glm::vec3 d(2.0f * corner->radius, Y, 0.0f);
	glm::vec3 e(2.0f * corner->radius, Y, -1.5f * corner->radius);
	glm::vec3 f(1.5f * corner->radius, Y, -2.0f * corner->radius);*/

	glm::vec3 a(0.0f, Y, -2.0 * corner->radius);
	glm::vec3 b(0.0f, Y, 0.0f);
	glm::vec3 c(2.0f * corner->radius, Y, 0.0f);
	glm::vec3 d(2.0f * corner->radius, Y, -1.5f * corner->radius);
	glm::vec3 e(1.5f * corner->radius, Y, -2.0f * corner->radius);

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	/*cornerMesh->addTriangleTex(a, b, f, normal);
	cornerMesh->addTriangleTex(f, b, c, normal);
	cornerMesh->addTriangleTex(f, c, e, normal);
	cornerMesh->addTriangleTex(e, c, d, normal);*/

	cornerMesh->addTriangleTex(a, b, e, normal);
	cornerMesh->addTriangleTex(e, b, d, normal);
	cornerMesh->addTriangleTex(d, b, c, normal);

	return cornerMesh;
}

std::shared_ptr<Mesh> GeometryFactory::GetFinishLine(glm::vec3 center, float size)
{
	auto finishLine = std::make_shared<Mesh>();

	glm::vec3 a(center.x - size / 2.0f, center.y, center.z - size / 2.0f);
	glm::vec3 b(center.x - size / 2.0f, center.y, center.z + size / 2.0f);
	glm::vec3 c(center.x + size / 2.0f, center.y, center.z + size / 2.0f);
	glm::vec3 d(center.x + size / 2.0f, center.y, center.z - size / 2.0f);

	finishLine->addQuadTex(a, b, c, d, glm::vec3(0.0f, 1.0f, 0.0f));

	return finishLine;
}
