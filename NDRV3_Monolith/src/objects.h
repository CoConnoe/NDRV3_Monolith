#pragma once

#include <vector>

enum ObjectType_t {
	MONOKUB,
	FISHIE
};

struct Object_t {
	ObjectType_t m_type;

	int m_rows;
	int m_cols;

	// 0 1 2
	// 3 4 5
	std::vector<bool> m_poslist;
};

Object_t createObject(ObjectType_t type, int rows, int cols, const std::vector<bool>& poslist);
std::vector<Object_t> getAllMonokubs();
std::vector<Object_t> getAllFishies();