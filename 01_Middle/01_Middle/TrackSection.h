#pragma once

#include <glm/glm.hpp>
#include "Parameters.h"
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
};

class Line : public TrackSection
{
public:
	enum class Orientation { HORIZONTAL, VERTICAL };

	Line(glm::vec3 start, glm::vec3 end, Orientation orientation, Direction direction):
		start(start), end(end), orientation(orientation), currentPosition(start), direction(direction)
	{ }

	// Inherited via TrackSection
	virtual PositionCalculationResult UpdatePosition(float timeElapsed, float speed) override;
	virtual void Reset() override;
	virtual glm::vec3 GetCurrentPosition() const override;
	virtual glm::vec3 GetEndPosition() const override;
protected:
	glm::vec3 start;
	glm::vec3 end;
	Orientation orientation;
	Direction direction;
	glm::vec3 currentPosition;
};

class Corner : public TrackSection
{
public:
	Corner(glm::vec3 center, float radius, int quadrant, Direction direction);

	// Inherited via TrackSection
	virtual PositionCalculationResult UpdatePosition(float timeElapsed, float speed) override;
	virtual void Reset() override;
	virtual glm::vec3 GetCurrentPosition() const override;
	virtual glm::vec3 GetEndPosition() const override;
protected:
	glm::vec3 center;
	float radius;
	int quadrant;
	Direction direction;
	float startAngle;
	float currentAngle;
};

class Track
{
public:
	std::vector<std::shared_ptr<TrackSection>> sections;
	void InitTrack();
	glm::vec3 GetPosition(float speed);
	bool IsInitialized() const { return isInitialized; }
private:
	int currentSection;
	float previousCalculation;
	bool isInitialized = false;
};
