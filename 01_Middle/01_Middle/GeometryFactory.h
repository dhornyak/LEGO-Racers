#pragma once

#include "Mesh_OGL3.h"
#include "Parameters.h"

#include <memory>

class GeometryFactory
{
public:
	static const float cubeWidthUnit;
	static const float thinCubeHeightUnit;

	static std::shared_ptr<Mesh> GetCuboid(glm::vec3 topLeftCorner, glm::vec3 bottomRightCorner);
	static std::shared_ptr<Mesh> GetCircle(glm::vec3 center, float radius);
	static std::shared_ptr<Mesh> GetCylinder(glm::vec3 bottomCenterPosition, float radius, float height, bool reverse = false);

	static std::shared_ptr<Mesh> GetKnob(glm::vec3 bottomCenterPosition);
	static std::shared_ptr<Mesh> GetLegoCube(int rows, int cols, CubeHeight height);
protected:
	static const int N;

	static const float knobHeight;
	static const float knobRadius;

	static const float cubeWidthOffset;
	static const float cubeTopWallThickness;
	static const float cubeSideWallThickness;
};