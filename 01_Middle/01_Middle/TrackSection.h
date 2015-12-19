#pragma once

#include "Parameters.h"
#include "GeometryFactory.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <SDL.h>

class PositionCalculationResult
{
public:
	PositionCalculationResult(glm::vec3 position, float timeRemains):
		position(position), timeRemains(timeRemains)
	{ }

	glm::vec3 position;
	float timeRemains;
};

class TrackSection
{
public:
	enum class Direction { PLUS, MINUS };

	virtual PositionCalculationResult UpdatePosition(float timeElapsed, float speed) = 0;
	virtual void Reset() = 0;
	virtual glm::vec3 GetCurrentPosition() const = 0;
	virtual glm::vec3 GetEndPosition() const = 0;
	
	virtual std::shared_ptr<Mesh> GetMesh() const = 0;
	virtual glm::vec3 TranslateMeshTo() const = 0;
	virtual float GetRotationAroundY() const = 0;
	virtual float GetDriveDirection() const = 0;
	virtual float GetCubeDirection() const = 0;

	static const float trackHalfWidth;
};

class Line : public TrackSection, public std::enable_shared_from_this<Line>
{
public:
	enum class Orientation { HORIZONTAL, VERTICAL };

	glm::vec3 start;
	glm::vec3 end;
	Orientation orientation;

	Line(glm::vec3 start, glm::vec3 end, Orientation orientation, Direction direction):
		start(start), end(end), orientation(orientation), currentPosition(start), direction(direction)
	{ }

	// Inherited via TrackSection
	virtual PositionCalculationResult UpdatePosition(float timeElapsed, float speed) override;
	virtual void Reset() override;
	virtual glm::vec3 GetCurrentPosition() const override;
	virtual glm::vec3 GetEndPosition() const override;

	virtual std::shared_ptr<Mesh> GetMesh() const override;
	virtual glm::vec3 TranslateMeshTo() const override;
	virtual float GetRotationAroundY() const override;
	virtual float GetDriveDirection() const override;
	virtual float GetCubeDirection() const override;
protected:
	Direction direction;
	glm::vec3 currentPosition;
};

class Corner : public TrackSection, public std::enable_shared_from_this<Corner>
{
public:
	float radius;
	int quadrant;
	glm::vec3 center;

	Corner(glm::vec3 center, float radius, int quadrant, Direction direction);

	// Inherited via TrackSection
	virtual PositionCalculationResult UpdatePosition(float timeElapsed, float speed) override;
	virtual void Reset() override;
	virtual glm::vec3 GetCurrentPosition() const override;
	virtual glm::vec3 GetEndPosition() const override;

	virtual std::shared_ptr<Mesh> GetMesh() const override;
	virtual glm::vec3 TranslateMeshTo() const override;
	virtual float GetRotationAroundY() const override;
	virtual float GetDriveDirection() const override;
	virtual float GetCubeDirection() const override;
protected:
	Direction direction;
	float startAngle;
	float currentAngle;
};

class Track
{
public:
	std::vector<std::shared_ptr<TrackSection>> sections;

	Track():
		currentSection(0)
	{ }

	void InitTrack();
	glm::vec3 GetPosition(float speed);
	float GetDriveDirection() const;
	float GetCubeDirection() const;
	bool IsInitialized() const { return isInitialized; }
private:
	int currentSection;
	float previousCalculation;
	bool isInitialized = false;
};
