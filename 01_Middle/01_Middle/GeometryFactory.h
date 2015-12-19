#pragma once

#include "Mesh_OGL3.h"
#include "Parameters.h"

#include <memory>

class Line;
class Corner;

class GeometryFactory
{
public:
	static const float cubeWidthUnit;
	static const float thinCubeHeightUnit;

	static std::shared_ptr<Mesh> GetCuboid(glm::vec3 topLeftCorner, glm::vec3 bottomRightCorner);
	static std::shared_ptr<Mesh> GetCircle(glm::vec3 center, glm::vec3 normal, float radius);
	static std::shared_ptr<Mesh> GetCylinder(glm::vec3 bottomCenterPosition, glm::vec3 direction, float radius, float height, bool reverse = false);
	
	static glm::vec3 CalculateSphereCoordinate(float x, float y, float radius);
	static std::shared_ptr<Mesh> GetSphere(glm::vec3 center, float radius);

	static std::shared_ptr<Mesh> GetKnob(glm::vec3 bottomCenterPosition);
	static std::shared_ptr<Mesh> GetLegoCube(int rows, int cols, CubeHeight height);
	static std::shared_ptr<Mesh> GetDriverTorso(glm::vec3 bottomCenterPosition);
	static std::shared_ptr<Mesh> GetDriver();
	static std::shared_ptr<Mesh> GetReflector();
	static std::shared_ptr<Mesh> GetWheel();

	static std::shared_ptr<Mesh> GetLineTrackMesh(std::shared_ptr<const Line> line, float halfWidth);
	static std::shared_ptr<Mesh> GetCornerTrackMesh(std::shared_ptr<const Corner> corner, float halfWidth);
protected:
	static const int N;

	static const float knobHeight;
	static const float knobRadius;

	static const float cubeWidthOffset;
	static const float cubeTopWallThickness;
	static const float cubeSideWallThickness;
};