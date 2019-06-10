#pragma once
#include <vector>

enum direction {north, east, south, west};

class Maze
{
private:
	int size;
	void recurse(int row, int col);
	std::vector<int> randOrder();
public:
	std::vector<std::vector<bool>> data;
	Maze(int size);
	void print();
};
