#include "stdafx.h"
#include <chrono>
#include <iostream>
#include <random>
#include <Windows.h>
#include "board.h"
#include "objects.h"

//#define __DEBUG

//const int MAX_TRIALS = 10; // TODO: increase as CPU permits
//const int MAX_DEPTH = 3; // TDOO: increase as CPU permits

const int MAX_TRIALS = 100; // TODO: increase as CPU permits
const int MAX_DEPTH = 30; // TDOO: increase as CPU permits

// assuming Mean difficulty
const int TOTAL_MONOKUBS = 2;
const int TOTAL_FISHIES = 7;

Board_t::Board_t(int maxrows, int maxcols)
	: m_rows(maxrows), m_cols(maxcols)
{
	m_array.resize(m_rows * m_cols, TILE_EMPTY);
}

void Board_t::initialize(const std::vector<int>& input)
{
	if (input.size() != m_rows * m_cols)
		throw std::runtime_error("Board_t::initialize input size error");

	for (int i = 0; i < input.size(); ++i) {
		switch (input[i]) {
		case 1:
			m_array[i] = TILE_WHITE;
			break;
		case 2:
			m_array[i] = TILE_PINK;
			break;
		case 3:
			m_array[i] = TILE_ORANGE;
			break;
		case 4:
			m_array[i] = TILE_BLUE;
			break;
		default:
			throw std::runtime_error("Board_t::initialize invalid input");
		}
	}
}

void tryToAddToBorder(std::set<int>& white_set, std::set<int>& black_set, int index, Tile_t color) {
	if (color == TILE_WHITE || color == TILE_PINK || color == TILE_ORANGE || color == TILE_BLUE) {
		std::set<int>::iterator it = white_set.find(index);
		if (it != white_set.end()) {
			black_set.insert(*it);
			white_set.erase(it);
		}
	}
}

void tryToAddToRegion(std::set<int>& white_set, std::set<int>& grey_set, int index, Tile_t color1, Tile_t color2) {
	if (color1 == color2) {
		std::set<int>::iterator it = white_set.find(index);
		if (it != white_set.end()) {
			grey_set.insert(*it);
			white_set.erase(it);
		}
	}
}

// TODO: refactor getAllRegions, getRegionAt, getBorder
std::vector<Region_t> Board_t::getAllRegions() const
{
	std::set<int> white_set, grey_set;
	for (int i = 0; i < m_array.size(); ++i)
		white_set.insert(i);

	std::vector<Region_t> rval;

	while (!white_set.empty()) {
		std::set<int> black_set;

		int start = *white_set.begin();
		white_set.erase(start);

		if (peek(start) != TILE_EMPTY) {
			grey_set.insert(start);

			while (!grey_set.empty()) {
				const int index = *grey_set.begin();
				const int row = getRow(index);
				const int col = getCol(index);
				const Tile_t color = peek(row, col);

				tryToAddToRegion(white_set, grey_set, getIndex(row - 1, col), color, peek(row - 1, col));
				tryToAddToRegion(white_set, grey_set, getIndex(row + 1, col), color, peek(row + 1, col));
				tryToAddToRegion(white_set, grey_set, getIndex(row, col - 1), color, peek(row, col - 1));
				tryToAddToRegion(white_set, grey_set, getIndex(row, col + 1), color, peek(row, col + 1));

				grey_set.erase(index);
				black_set.insert(index);
			}

			if (black_set.size() > 0) { // don't add single tile regions
				Region_t newRegion;
				newRegion.m_type = peek(*black_set.begin());
				newRegion.m_coords = black_set;

				rval.push_back(newRegion);
			}
		}
	}

	return rval;
}

Region_t Board_t::calculateBestMove() const
{
	int bestRegionId = -1;
	int bestScore = -1;

	static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::default_random_engine generator(seed);

	std::vector<Region_t> regionlist = getAllRegions();
	std::vector<ObjectLoc_t> possibleHiddenMonokubs = possibleHiddenMonokubLocs();
	std::vector<ObjectLoc_t> possibleHiddenFishies = possibleHiddenFishieLocs();

	for (int trialnum = 0; trialnum < MAX_TRIALS; ++trialnum) {
#ifdef __DEBUG
		std::cout << "DEBUG TRIAL:" << trialnum << std::endl;
#endif

		// pick where the stuff is hidden for this trial
		std::vector<ObjectLoc_t> trialMonokubLocs;
		std::vector<ObjectLoc_t> trialFishieLocs;

		for (int r = 0; r < regionlist.size(); ++r) {
			if (regionlist[r].m_type == TILE_MONOKUB) {
				std::vector<ObjectLoc_t> possibleLoc = possibleMonokubLocs(regionlist[r]);
				if (!possibleLoc.empty()) {
					std::uniform_int_distribution<int> distribution(0, possibleLoc.size() - 1);
					trialMonokubLocs.push_back(possibleLoc[distribution(generator)]);
				}
			}
			else if (regionlist[r].m_type == TILE_FISHIE) {
				std::vector<ObjectLoc_t> possibleLoc = possibleFishieLocs(regionlist[r]);
				if (!possibleLoc.empty()) {
					std::uniform_int_distribution<int> distribution(0, possibleLoc.size() - 1);
					trialFishieLocs.push_back(possibleLoc[distribution(generator)]);
				}
			}
		}

		// TODO: Ensure no objects overlap
		while (trialMonokubLocs.size() < TOTAL_MONOKUBS) {
			std::uniform_int_distribution<int> distribution(0, possibleHiddenMonokubs.size() - 1);
			trialMonokubLocs.push_back(possibleHiddenMonokubs[distribution(generator)]);
		}
		while (trialFishieLocs.size() < TOTAL_FISHIES) {
			std::uniform_int_distribution<int> distribution(0, possibleHiddenFishies.size() - 1);
			trialFishieLocs.push_back(possibleHiddenFishies[distribution(generator)]);
		}

		// examine each possible move
		for (int r = 0; r < regionlist.size(); ++r) {
			if ((regionlist[r].m_type == TILE_WHITE || regionlist[r].m_type == TILE_PINK || regionlist[r].m_type == TILE_ORANGE || regionlist[r].m_type == TILE_BLUE) && regionlist[r].m_coords.size() > 1) {
#ifdef __DEBUG
				//std::cout << "DEBUG REGION:" << r << " OF " << regionlist.size() << std::endl;
#endif

				Board_t futureboard(*this);
				futureboard.breakRegion(regionlist[r]);
				futureboard.applyObjectLocations(trialMonokubLocs, trialFishieLocs);
				//int score = futureboard.calculateOptimalScore(trialMonokubLocs, trialFishieLocs, MAX_DEPTH);
				int score = futureboard.mashButtonsScore(trialMonokubLocs, trialFishieLocs, generator, MAX_DEPTH);

				if (score > bestScore) {
					bestScore = score;
					bestRegionId = r;
				}
			}
		}
	}

	if (bestRegionId == -1)
		return Region_t();

	return regionlist[bestRegionId];
}

int Board_t::calculateScore() const
{
	int score = 0;

	for (int i = 0; i < m_array.size(); ++i) {
		Tile_t color = peek(i);
		if (color != TILE_WHITE && color != TILE_PINK && color != TILE_ORANGE && color != TILE_BLUE)
			score += 5;
	}

	std::vector<Object_t> allMonokubs = getAllMonokubs();
	std::vector<Object_t> allFishies = getAllFishies();

	std::vector<Region_t> regionlist = getAllRegions();
	for (int r = 0; r < regionlist.size(); ++r) {
		if (regionlist[r].m_type == TILE_MONOKUB) {
			std::vector<ObjectLoc_t> possibleLoc = possibleMonokubLocs(regionlist[r]);
#ifdef __DEBUG
			std::cout << "DEBUG MONO R:" << r << " Size:" << possibleLoc.size() << std::endl;
#endif
			if (possibleLoc.size() == 1) {
				int configId = possibleLoc[0].m_config_id;
				int originIndex = possibleLoc[0].m_origin;
				if (isCompletelyRevealed(allMonokubs[configId], originIndex)) {
					score += 1000;
				}
			}
		}
		else if (regionlist[r].m_type == TILE_FISHIE) {
			std::vector<ObjectLoc_t> possibleLoc = possibleFishieLocs(regionlist[r]);
#ifdef __DEBUG
			std::cout << "DEBUG FISH R:" << r << " Size:" << possibleLoc.size() << std::endl;
#endif
			if (possibleLoc.size() == 1) {
				int configId = possibleLoc[0].m_config_id;
				int originIndex = possibleLoc[0].m_origin;
				if (isCompletelyRevealed(allFishies[configId], originIndex)) {
					score += 500;
				}
			}
		}
	}

	return score;
}

Region_t Board_t::getRegionAt(int row, int col) const
{
	std::set<int> white_set, grey_set, black_set;
	for (int i = 0; i < m_array.size(); ++i)
		white_set.insert(i);

	Region_t rval;

	int start = getIndex(row, col);
	white_set.erase(start);
	grey_set.insert(start);

	while (!grey_set.empty()) {
		const int index = *grey_set.begin();
		const int grey_row = getRow(index);
		const int grey_col = getCol(index);
		const Tile_t color = peek(grey_row, grey_col);

		tryToAddToRegion(white_set, grey_set, getIndex(grey_row - 1, grey_col), color, peek(grey_row - 1, grey_col));
		tryToAddToRegion(white_set, grey_set, getIndex(grey_row + 1, grey_col), color, peek(grey_row + 1, grey_col));
		tryToAddToRegion(white_set, grey_set, getIndex(grey_row, grey_col - 1), color, peek(grey_row, grey_col - 1));
		tryToAddToRegion(white_set, grey_set, getIndex(grey_row, grey_col + 1), color, peek(grey_row, grey_col + 1));

		grey_set.erase(index);
		black_set.insert(index);
	}

	if (black_set.size() > 0) { // single tile regions are okay
		rval.m_type = peek(row, col);
		rval.m_coords = black_set;
	}
	else {
		rval.m_type = NOT_TILE_OOB;
	}
	
	return rval;
}

Region_t Board_t::getBorder(const Region_t& region) const
{
	Region_t rval;
	rval.m_type = NOT_TILE_BORDER;

	if (region.m_type == TILE_WHITE || region.m_type == TILE_PINK || region.m_type == TILE_ORANGE || region.m_type == TILE_BLUE) {
		std::set<int> white_set, grey_set, black_set;
		for (int i = 0; i < m_array.size(); ++i)
			white_set.insert(i);

		for (std::set<int>::iterator it = region.m_coords.begin(); it != region.m_coords.end(); ++it) {
			white_set.erase(*it);
			grey_set.insert(*it);
		}

		while (!grey_set.empty()) {
			const int index = *grey_set.begin();
			const int row = getRow(index);
			const int col = getCol(index);

			tryToAddToBorder(white_set, black_set, getIndex(row - 1, col), peek(row - 1, col));
			tryToAddToBorder(white_set, black_set, getIndex(row + 1, col), peek(row + 1, col));
			tryToAddToBorder(white_set, black_set, getIndex(row, col - 1), peek(row, col - 1));
			tryToAddToBorder(white_set, black_set, getIndex(row, col + 1), peek(row, col + 1));

			grey_set.erase(index);
		}

		rval.m_coords = black_set;
	}

	return rval;
}

Tile_t Board_t::peek(int row, int col) const
{
	return peek(getIndex(row, col));
}

Tile_t Board_t::peek(int index) const
{
	if (index < 0 || index >= m_array.size())
		return NOT_TILE_OOB;

	return m_array[index];
}

int Board_t::getRow(int index) const
{
	if (index < 0 || index >= m_array.size())
		return -1;

	return floor(index / m_cols);
}

int Board_t::getCol(int index) const
{
	if (index < 0 || index >= m_array.size())
		return -1;

	return index % m_cols;
}

int Board_t::getIndex(int row, int col) const
{
	if (row < 0 || row >= m_rows || col < 0 || col >= m_cols)
		return -1;

	return row * m_cols + col;
}

bool Board_t::breakRegion(const Region_t& region, bool output)
{
	if (region.m_coords.size() == 0 || !(region.m_type == TILE_WHITE || region.m_type == TILE_PINK || region.m_type == TILE_ORANGE || region.m_type == TILE_BLUE)) {
		std::cout << "ERROR: Invalid region!" << std::endl;
		return false;
	}
	else if (region.m_coords.size() == 1) {
		std::cout << "ERROR: Cannot break single blocks!" << std::endl;
		return false;
	}
	else {
		Region_t border = getBorder(region);

		// clear selected region
		for (std::set<int>::iterator it = region.m_coords.begin(); it != region.m_coords.end(); ++it) {
			if (output)
				std::cout << "(" << getRow(*it) << "," << getCol(*it) << ") BREAK" << std::endl;

			m_array[*it] = TILE_EMPTY;
		}

		// cycle border
		for (std::set<int>::iterator it = border.m_coords.begin(); it != border.m_coords.end(); ++it) {
			switch (m_array[*it]) {
			case TILE_WHITE:
				m_array[*it] = TILE_PINK;
				break;
			case TILE_PINK:
				m_array[*it] = TILE_ORANGE;
				break;
			case TILE_ORANGE:
				m_array[*it] = TILE_BLUE;
				break;
			case TILE_BLUE:
				m_array[*it] = TILE_WHITE;
				break;
			}
		}

		return true;
	}
}

int Board_t::calculateOptimalScore(const std::vector<ObjectLoc_t>& monokubLocs, const std::vector<ObjectLoc_t>& fishieLocs, int depth)
{
	int bestScore = -1;

	/*#ifdef __DEBUG
	std::cout << "DEBUG DEPTH:" << MAX_DEPTH - depth << std::endl;
	#endif*/

	std::vector<Region_t> regionlist = getAllRegions();
	if (regionlist.empty() || depth <= 0)
		return calculateScore();

	for (int r = 0; r < regionlist.size(); ++r) {
		if ((regionlist[r].m_type == TILE_WHITE || regionlist[r].m_type == TILE_PINK || regionlist[r].m_type == TILE_ORANGE || regionlist[r].m_type == TILE_BLUE) && regionlist[r].m_coords.size() > 1) {
			Board_t futureboard(*this);
			futureboard.breakRegion(regionlist[r]);
			futureboard.applyObjectLocations(monokubLocs, fishieLocs);
			int score = futureboard.calculateOptimalScore(monokubLocs, fishieLocs, depth - 1);

			if (score > bestScore) {
				bestScore = score;
			}
		}
	}

	return bestScore;
}

int Board_t::mashButtonsScore(const std::vector<ObjectLoc_t>& monokubLocs, const std::vector<ObjectLoc_t>& fishieLocs, std::default_random_engine& generator, int depth)
{
	/*#ifdef __DEBUG
	std::cout << "DEBUG DEPTH:" << MAX_DEPTH - depth << std::endl;
	#endif*/

	std::vector<Region_t> regionlist = getAllRegions();
	std::vector<int> regionChoices;

	for (int r = 0; r < regionlist.size(); ++r) {
		if ((regionlist[r].m_type == TILE_WHITE || regionlist[r].m_type == TILE_PINK || regionlist[r].m_type == TILE_ORANGE || regionlist[r].m_type == TILE_BLUE) && regionlist[r].m_coords.size() > 1) {
			regionChoices.push_back(r);
		}
	}

	if (regionChoices.empty() || depth <= 0)
		return calculateScore();

	std::uniform_int_distribution<int> distribution(0, regionChoices.size() - 1);
		
	Board_t futureboard(*this);
	futureboard.breakRegion(regionlist[regionChoices[distribution(generator)]]);
	futureboard.applyObjectLocations(monokubLocs, fishieLocs);
	return futureboard.mashButtonsScore(monokubLocs, fishieLocs, generator, depth - 1);
}

std::vector<ObjectLoc_t> Board_t::possibleMonokubLocs(const Region_t & region) const
{
	std::vector<Object_t> allMonokubs = getAllMonokubs();
	return possibleObjectLocs(region, allMonokubs);
}

std::vector<ObjectLoc_t> Board_t::possibleHiddenMonokubLocs() const
{
	std::vector<Object_t> allMonokubs = getAllMonokubs();
	return possibleHiddenObjectLocs(allMonokubs);
}

std::vector<ObjectLoc_t> Board_t::possibleFishieLocs(const Region_t & region) const
{
	std::vector<Object_t> allFishies = getAllFishies();
	return possibleObjectLocs(region, allFishies);
}

std::vector<ObjectLoc_t> Board_t::possibleHiddenFishieLocs() const
{
	std::vector<Object_t> allFishies = getAllFishies();
	return possibleHiddenObjectLocs(allFishies);
}

std::vector<ObjectLoc_t> Board_t::possibleObjectLocs(const Region_t & region, const std::vector<Object_t>& allObjects) const
{
	std::vector<ObjectLoc_t> rval;

	// get the bounds of the region
	int top = m_rows - 1, right = 0, bottom = 0, left = m_cols - 1;
	for (std::set<int>::iterator it = region.m_coords.begin(); it != region.m_coords.end(); ++it) {
		int regRow = getRow(*it);
		int regCol = getCol(*it);

		if (top > regRow)
			top = regRow;
		if (bottom < regRow)
			bottom = regRow;
		if (left > regCol)
			left = regCol;
		if (right < regCol)
			right = regCol;
	}

	int height = bottom - top + 1;
	int width = right - left + 1;

	for (int obj = 0; obj < allObjects.size(); ++obj) {
		// take the object and "wiggle" it around the exposed region 
		for (int row = top - allObjects[obj].m_rows + height; row <= top; ++row) {
			for (int col = left - allObjects[obj].m_cols + width; col <= left; ++col) {
				if (couldBeHiddenHere(allObjects[obj], getIndex(row, col), false)) {
					ObjectLoc_t objectloc;
					objectloc.m_origin = getIndex(row, col);
					objectloc.m_config_id = obj;

					rval.push_back(objectloc);
				}
			}
		}
	}

	return rval;
}

std::vector<ObjectLoc_t> Board_t::possibleHiddenObjectLocs(const std::vector<Object_t>& allObjects) const
{
	std::vector<ObjectLoc_t> rval;

	for (int obj = 0; obj < allObjects.size(); ++obj) {
		for (int index = 0; index < m_array.size(); ++index) {
			if (couldBeHiddenHere(allObjects[obj], index, true)) {
				ObjectLoc_t objectloc;
				objectloc.m_origin = index;
				objectloc.m_config_id = obj;

				rval.push_back(objectloc);
			}
		}
	}

	return rval;
}

void Board_t::applyObjectLocations(const std::vector<ObjectLoc_t>& monokubLocs, const std::vector<ObjectLoc_t>& fishieLocs)
{
	std::vector<Object_t> allMonokubs = getAllMonokubs();
	for (int m = 0; m < monokubLocs.size(); ++m) {
		Object_t& monokub = allMonokubs[monokubLocs[m].m_config_id];

		int top = getRow(monokubLocs[m].m_origin);
		int bottom = top + monokub.m_rows - 1;
		int left = getCol(monokubLocs[m].m_origin);
		int right = left + monokub.m_cols - 1;
		int objectIndex = 0;

		for (int row = top; row <= bottom; ++row) {
			for (int col = left; col <= right; ++col, ++objectIndex) {
				if (monokub.m_poslist[objectIndex] && m_array[getIndex(row, col)] == TILE_EMPTY) {
					m_array[getIndex(row, col)] = TILE_MONOKUB;
				}
			}
		}
	}

	std::vector<Object_t> allFishies = getAllFishies();
	for (int m = 0; m < fishieLocs.size(); ++m) {
		Object_t& fishie = allFishies[fishieLocs[m].m_config_id];

		int top = getRow(fishieLocs[m].m_origin);
		int bottom = top + fishie.m_rows - 1;
		int left = getCol(fishieLocs[m].m_origin);
		int right = left + fishie.m_cols - 1;
		int objectIndex = 0;

		for (int row = top; row <= bottom; ++row) {
			for (int col = left; col <= right; ++col, ++objectIndex) {
				if (fishie.m_poslist[objectIndex] && m_array[getIndex(row, col)] == TILE_EMPTY) {
					m_array[getIndex(row, col)] = TILE_FISHIE;
				}
			}
		}
	}
}

// TODO: refactor couldBeHiddenHere and isCompletelyRevealed
bool Board_t::couldBeHiddenHere(const Object_t & object, int originIndex, bool fullyHidden) const
{
	Tile_t type;
	if (object.m_type == MONOKUB) {
		type = TILE_MONOKUB;
	}
	else {
		type = TILE_FISHIE;
	}

	int originRow = getRow(originIndex);
	int originCol = getCol(originIndex);
	int objectIndex = 0;

	bool rval = true;

	for (int row = originRow; row < originRow + object.m_rows; ++row) {
		for (int col = originCol; col < originCol + object.m_cols; ++col, ++objectIndex) {
			if (object.m_poslist[objectIndex]) {
				Tile_t peekTile = peek(row, col);
				if (peekTile != TILE_WHITE && peekTile != TILE_PINK && peekTile != TILE_ORANGE && peekTile != TILE_BLUE && (fullyHidden || peekTile != type))
					rval = false;
			}
		}
	}

	return rval;
}

bool Board_t::isCompletelyRevealed(const Object_t & object, int originIndex) const
{
	Tile_t type;
	if (object.m_type == MONOKUB) {
		type = TILE_MONOKUB;
	}
	else {
		type = TILE_FISHIE;
	}

	int originRow = getRow(originIndex);
	int originCol = getCol(originIndex);
	int objectIndex = 0;

	bool rval = true;

	for (int row = originRow; row < originRow + object.m_rows; ++row) {
		for (int col = originCol; col < originCol + object.m_cols; ++col, ++objectIndex) {
			if (object.m_poslist[objectIndex] && peek(row, col) != type) {
				rval = false;
			}
		}
	}

	return rval;
}

bool Board_t::breakRegionAt(int row, int col, bool output)
{
	Region_t region = getRegionAt(row, col);
	return breakRegion(region, output);
}

bool Board_t::modify(int row, int col, Tile_t newType)
{
	int index = getIndex(row, col);

	if (index >= 0 && index < m_array.size()) {
		if (m_array[index] != newType) {
			m_array[index] = newType;
			return true;
		}
	}
	
	return false;
}

void Board_t::print(const Region_t& bestRegion) const
{
	// TODO: change char(219) to * if this is the region I should choose next
	HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < m_array.size(); ++i) {
		if (i % m_cols == 0 && i > 0)
			std::cout << std::endl;

		bool inBestRegion = (bestRegion.m_coords.find(i) != bestRegion.m_coords.end());
		char tilechar = (inBestRegion ? '*' : char(219));

		switch (m_array[i]) {
		case TILE_EMPTY:
			std::cout << ' ';
			break;
		case TILE_WHITE:
			SetConsoleTextAttribute(stdhandle, 15);
			std::cout << tilechar;
			break;
		case TILE_PINK:
			SetConsoleTextAttribute(stdhandle, 13);
			std::cout << tilechar;
			break;
		case TILE_ORANGE:
			SetConsoleTextAttribute(stdhandle, 14);
			std::cout << tilechar;
			break;
		case TILE_BLUE:
			SetConsoleTextAttribute(stdhandle, 9);
			std::cout << tilechar;
			break;
		case TILE_MONOKUB:
			SetConsoleTextAttribute(stdhandle, 7);
			std::cout << 'M';
			break;
		case TILE_FISHIE:
			SetConsoleTextAttribute(stdhandle, 7);
			std::cout << 'F';
			break;
		}
	}

	SetConsoleTextAttribute(stdhandle, 7);
	std::cout << std::endl;
}
