#include "stdafx.h"
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

void tryToAddToRegion(std::set<int>& white_set, std::set<int>& grey_set, int index) {
	std::set<int>::iterator it = white_set.find(index);
	if (it != white_set.end()) {
		grey_set.insert(*it);
		white_set.erase(it);
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
		grey_set.insert(start);

		while (!grey_set.empty()) {
			const int index = *grey_set.begin();
			const int row = getRow(index);
			const int col = getCol(index);
			const Tile_t color = peek(row, col);

			tryToAddToRegion(white_set, grey_set, getIndex(row - 1, col));
			tryToAddToRegion(white_set, grey_set, getIndex(row + 1, col));
			tryToAddToRegion(white_set, grey_set, getIndex(row, col - 1));
			tryToAddToRegion(white_set, grey_set, getIndex(row, col + 1));

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

	return rval;
}

Region_t Board_t::getRegionAt(int row, int col) const
{
	return Region_t();
}


