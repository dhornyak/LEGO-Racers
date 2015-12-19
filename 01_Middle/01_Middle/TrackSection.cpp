#include "TrackSection.h"

const float TrackSection::trackHalfWidth = 4.0f * GeometryFactory::cubeWidthUnit;

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

std::shared_ptr<Mesh> Line::GetMesh() const
{
	return GeometryFactory::GetLineTrackMesh(shared_from_this(), trackHalfWidth);
}

glm::vec3 Line::TranslateMeshTo() const
{
	return start;
}

float Line::GetRotationAroundY() const
{
	return 0.0f;
}

float Line::GetDriveDirection() const
{
	float angle = 0.0f;

	switch (orientation)
	{
	case Orientation::HORIZONTAL: angle = -90.0f;
		break;
	case Orientation::VERTICAL: angle = 0.0f;
		break;
	default:
		break;
	}

	return angle;
}

float Line::GetCubeDirection() const
{
	return GetDriveDirection();
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

std::shared_ptr<Mesh> Corner::GetMesh() const
{
	return GeometryFactory::GetCornerTrackMesh(shared_from_this(), trackHalfWidth);
}

glm::vec3 Corner::TranslateMeshTo() const
{
	return center;
}

float Corner::GetRotationAroundY() const
{
	return (quadrant - 1) * 90.0f;
}

float Corner::GetDriveDirection() const
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

	return angleInQuadrant * 180.0f / M_PI;
}

float Corner::GetCubeDirection() const
{
	return (direction == Direction::MINUS) ? GetDriveDirection() + 180.0f : GetDriveDirection();
}

//////////////////////////
// TRACK
//////////////////////////

void Track::InitTrack()
{
	previousCalculation = SDL_GetTicks() / 1000.0f;
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
			else
			{
				isFinished = true;
			}
		}

		previousCalculation = currentTick;
		return result.position;
	}

	previousCalculation = currentTick;
	return sections[currentSection - 1]->GetEndPosition();
}

float Track::GetCubeDirection() const
{
	int sectionToQuery = (currentSection < sections.size()) ? currentSection : currentSection - 1;
	return sections[sectionToQuery]->GetCubeDirection();
}

float Track::GetDriveDirection() const
{
	int sectionToQuery = (currentSection < sections.size()) ? currentSection : currentSection - 1;
	return sections[sectionToQuery]->GetDriveDirection();
}
