#include <objects.h>
#include <vector>
#include "stdafx.h"

Object_t createObject(ObjectType_t type, int rows, int cols, const std::vector<bool>& poslist)
{
	Object_t rval;
	rval.m_type = type;
	rval.m_rows = rows;
	rval.m_cols = cols;
	rval.m_poslist = poslist;

	return rval;
}

std::vector<Object_t> getAllMonokubs() {
	static std::vector<Object_t> g_monokubs;
	if (!g_monokubs.empty())
		return g_monokubs;

	// MM
	// MM
	// MM
	// MM ...and rotated
	g_monokubs.push_back(createObject(MONOKUB, 4, 2, { 1, 1, 1, 1, 1, 1, 1, 1 }));
	g_monokubs.push_back(createObject(MONOKUB, 2, 4, { 1, 1, 1, 1, 1, 1, 1, 1 }));

	// MM
	// MMM
	// MMM
	// MMM ...and rotated
	g_monokubs.push_back(createObject(MONOKUB, 4, 3, { 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 }));
	g_monokubs.push_back(createObject(MONOKUB, 3, 4, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 }));
	g_monokubs.push_back(createObject(MONOKUB, 4, 3, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 }));
	g_monokubs.push_back(createObject(MONOKUB, 3, 4, { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }));

	// MMM
	// MMMM
	// MMMM
	//  MMM ...and rotated
	g_monokubs.push_back(createObject(MONOKUB, 4, 4, { 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 }));
	g_monokubs.push_back(createObject(MONOKUB, 4, 4, { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 }));

	return g_monokubs;
}

std::vector<Object_t> getAllFishies()
{
	static std::vector<Object_t> g_fishies;
	if (!g_fishies.empty())
		return g_fishies;

	// FF
	// FF
	// FF
	// FF ...and rotated
	g_fishies.push_back(createObject(MONOKUB, 4, 2, { 1, 1, 1, 1, 1, 1, 1, 1 }));
	g_fishies.push_back(createObject(MONOKUB, 2, 4, { 1, 1, 1, 1, 1, 1, 1, 1 }));

	// FF
	// FFF
	//  FFF
	//   FF ...and rotated
	g_fishies.push_back(createObject(MONOKUB, 4, 4, { 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1 }));
	g_fishies.push_back(createObject(MONOKUB, 4, 4, { 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0 }));

	// FF
	// FFF
	//  FF
	//   F ...and rotated
	g_fishies.push_back(createObject(MONOKUB, 4, 3, { 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1 }));
	g_fishies.push_back(createObject(MONOKUB, 3, 4, { 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0 }));
	g_fishies.push_back(createObject(MONOKUB, 4, 3, { 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1 }));
	g_fishies.push_back(createObject(MONOKUB, 3, 4, { 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0 }));

	return g_fishies;
}
