#include "TrackSection.h"

//////////////////////////
// LINE
//////////////////////////

PositionCalculationResult Line::UpdatePosition(float timeElapsed, float speed)
{
	float distanceToGo = timeElapsed * speed;
	float currentCoordinateValue, endCoordinateValue;
	float distanceRemaining;

	// Determine start and end coordinate.
	switch (orientation)
	{
	case Orientation::HORIZONTAL: distanceRemaining = abs(currentPosition.x - end.x);
		break;
	case Orientation::VERTICAL: distanceRemaining = abs(currentPosition.z - end.z);
		break;
	default:
		break;
	}

	// Calculate next position.
	if (distanceToGo > distanceRemaining)
	{
		float timeRemains = timeElapsed - (distanceRemaining / speed);
		return PositionCalculationResult(currentPosition, timeRemains);
	}
	else
	{
		switch (orientation)
		{
		case Orientation::HORIZONTAL: 
			switch (direction)
			{
			case TrackSection::Direction::MINUS:
				currentPosition.x -= distanceToGo;
				break;
			case TrackSection::Direction::PLUS:
				currentPosition.x += distanceToGo;
				break;
			default:
				break;
			} 
			break;
		case Orientation::VERTICAL:
			switch (direction)
			{
			case TrackSection::Direction::MINUS:
				currentPosition.z -= distanceToGo;
				break;
			case TrackSection::Direction::PLUS:
				currentPosition.z += distanceToGo;
				break;
			default:
				break;
			}
		default:
			break;
		}

		return PositionCalculationResult(currentPosition, 0.0f);
	}
}

void Line::Reset()
{
	currentPosition = start;
}

glm::vec3 Line::GetCurrentPosition() const
{
	return currentPosition;
}

glm::vec3 Line::GetEndPosition() const
{
	return end;
}

//////////////////////////
// CORNER
//////////////////////////

Corner::Corner(glm::vec3 center, float radius, int quadrant, Direction direction) :
	center(center), radius(radius), quadrant(quadrant), direction(direction), TrackSection()
{
	startAngle = 0.0f;
	currentAngle = startAngle;
}

PositionCalculationResult Corner::UpdatePosition(float timeElapsed, float speed)
{
	float currentDistance = currentAngle * radius;
	float distanceRemaining = M_PI * radius / 2.0f - currentDistance;
	float distanceToGo = timeElapsed * speed;

	if (distanceToGo > distanceRemaining)
	{
		float timeRemains = timeElapsed - (distanceRemaining / speed);
		return PositionCalculationResult(GetCurrentPosition(), timeRemains);
	}
	else
	{
		currentAngle += distanceToGo / radius; // angle to rotate
		return PositionCalculationResult(GetCurrentPosition(), 0.0f);
	}
}

void Corner::Reset()
{
	currentAngle = startAngle;
}

glm::vec3 Corner::GetCurrentPosition() const
{
	float angleInQuadrant;

	switch (direction)
	{
	case Direction::MINUS:
		angleInQuadrant = quadrant * M_PI / 2.0f - currentAngle;
		break;
	case Direction::PLUS:
		angleInQuadrant = (quadrant - 1) * M_PI / 2.0f + currentAngle;
		break;
	default:
		break;
	}

	return glm::vec3(radius * cos(angleInQuadrant), center.y, -radius * sin(angleInQuadrant)) + center;
}

glm::vec3 Corner::GetEndPosition() const
{
	float endAngle = (direction == Direction::PLUS) ? quadrant * M_PI / 2.0f : (quadrant - 1) * M_PI / 2.0f;
	return glm::vec3(radius * cos(endAngle), center.y, -radius * sin(endAngle)) + center;
}

//////////////////////////
// TRACK
//////////////////////////

void Track::InitTrack()
{
	previousCalculation = SDL_GetTicks() / 1000.0f;
	currentSection = 0;
	isInitialized = true;
}

glm::vec3 Track::GetPosition(float speed)
{
	float currentTick = SDL_GetTicks() / 1000.0f;
	float timeElapsed = currentTick - previousCalculation;

	if (currentSection < sections.size())
	{
		auto result = sections[currentSection]->UpdatePosition(timeElapsed, speed);
		if (result.timeRemains != 0.0f)
		{
			++currentSection;
			
			if (currentSection < sections.size())
			{
				result = sections[currentSection]->UpdatePosition(timeElapsed, speed);
				previousCalculation = currentTick;
				return result.position;
			}	
		}

		previousCalculation = currentTick;
		return result.position;
	}

	previousCalculation = currentTick;
	return sections[currentSection - 1]->GetEndPosition();
}