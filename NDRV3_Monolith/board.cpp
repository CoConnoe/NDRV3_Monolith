#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include "board.h"

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

			if (black_set.size() > 1) { // don't add single tile regions
				Region_t newRegion;
				newRegion.m_type = peek(*black_set.begin());
				newRegion.m_coords = black_set;

				rval.push_back(newRegion);
			}
		}
	}

	return rval;
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

Region_t Board_t::getBorder(Region_t region) const
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

bool Board_t::breakRegion(Region_t region)
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

bool Board_t::breakRegionAt(int row, int col)
{
	Region_t region = getRegionAt(row, col);
	return breakRegion(region);
}

void Board_t::print()
{
	// TODO: change char(219) to * if this is the region I should choose next
	HANDLE stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < m_array.size(); ++i) {
		if (i % m_cols == 0 && i > 0)
			std::cout << std::endl;

		switch (m_array[i]) {
		case TILE_EMPTY:
			std::cout << ' ';
			break;
		case TILE_WHITE:
			SetConsoleTextAttribute(stdhandle, 15);
			std::cout << char(219);
			break;
		case TILE_PINK:
			SetConsoleTextAttribute(stdhandle, 13);
			std::cout << char(219);
			break;
		case TILE_ORANGE:
			SetConsoleTextAttribute(stdhandle, 14);
			std::cout << char(219);
			break;
		case TILE_BLUE:
			SetConsoleTextAttribute(stdhandle, 9);
			std::cout << char(219);
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
