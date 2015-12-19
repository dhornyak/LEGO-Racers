#pragma once

#include "Parameters.h"
#include "CubeSize.h"
#include "Mesh_OGL3.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif // M_PI

class Position
{
public:
	Position(int row, int col, int height):
			row(row), col(col), height(height)
	{ }

	int row;
	int col;
	int height;
};

class Cube
{
public:
	Cube(Position position, std::map<CubeColor, GLuint>::iterator color, float rotation, std::map<CubeSize, std::shared_ptr<Mesh>>::iterator mesh):
		position(position), color(color), rotation(rotation), mesh(mesh)
	{ }

	Position position;
	std::map<CubeColor, GLuint>::iterator color;
	float rotation;
	std::map<CubeSize, std::shared_ptr<Mesh>>::iterator mesh;
};

class CubeGroup
{
public:
	std::vector<std::shared_ptr<Cube>> cubes;
};
