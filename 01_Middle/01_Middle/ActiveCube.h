#pragma once

#include "Parameters.h"
#include "CubeSize.h"
#include "Mesh_OGL3.h"

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
	Cube(Position position, CubeColor color, float rotation, std::map<CubeSize, std::shared_ptr<Mesh>>::iterator mesh):
		position(position), color(color), rotation(rotation), mesh(mesh)
	{ }

	Position position;
	CubeColor color;
	float rotation;
	std::map<CubeSize, std::shared_ptr<Mesh>>::iterator mesh;
};
