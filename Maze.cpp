#include "Maze.h"
#include <algorithm>
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <iostream>

Maze::Maze(int size)
{
	// true is cell, false wall
	this->size = size;
	data = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
	data[1][1] = 1;

	recurse(1, 1);
}

void Maze::recurse(int row, int col)
{
	std::vector<int> order = randOrder();
	for (int i : order)
	{
		// stops when no free cells. first check space in map, then if 2 slots over is a cell or wall
		switch (static_cast<direction>(i))
		{
		case north:

			if (row - 2 > 0 && !data[row - 2][col]) // not yet cell, make cell
			{
				data[row - 1][col] = 1;
				data[row - 2][col] = 1;
				recurse(row - 2, col);
			}
			break;
		case east:
			if (col + 2 < size - 1 && !data[row][col + 2])
			{
				data[row][col + 1] = 1;
				data[row][col + 2] = 1;
				recurse(row, col + 2);
			}
			break;
		case south:
			if (row + 2 < size - 1 && !data[row + 2][col])
			{
				data[row + 1][col] = 1;
				data[row + 2][col] = 1;
				recurse(row + 2, col);
			}
			break;
		case west:
			if (col - 2 > 2 && !data[row][col - 2])
			{
				data[row][col - 1] = 1;
				data[row][col - 2] = 1;
				recurse(row, col - 2);
			}
			break;
		}
	}
}

std::vector<int> Maze::randOrder()
{
	// gives random ordering of cardinal directions n...w => 0...3
	std::vector<int> out = std::vector<int>();
	for (int i = 0; i < 4; i++)
	{
		out.push_back(i);
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(out.begin(), out.end(), std::default_random_engine(seed));
	return out;
}

void Maze::print()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			std::cout << (data[i][j] ? " " : "x");
		}
		std::cout << "\n";
	}
}
