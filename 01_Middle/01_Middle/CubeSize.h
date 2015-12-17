#pragma once

#include "Parameters.h"

struct CubeSize
{
public:
	CubeSize(int rows, int cols, int height) :
		rows(rows), cols(cols), height(height)
	{ }

	bool operator<(CubeSize const& rhs) const
	{
		return 
			(rows < rhs.rows) ||
			(rows == rhs.rows && cols < rhs.cols) ||
			(rows == rhs.rows && cols == rhs.cols && height < rhs.height);
	}

	int rows;
	int cols;
	int height;
};
