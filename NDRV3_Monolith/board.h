#pragma once

#include <set>
#include <vector>

enum Tile_t {
	TILE_EMPTY,
	TILE_WHITE,		// one dot
	TILE_PINK,		// two dot
	TILE_ORANGE,	// three dot
	TILE_BLUE,		// four dot
	TILE_MONOKUB,
	TILE_FISHIE,

	NOT_TILE_OOB,	// special code for off the map
	NOT_TILE_BORDER // special code for all tiles bordering a region (color change)
};

struct Region_t {
	Tile_t m_type;
	std::set<int> m_coords;
};

struct ObjectLoc_t {
	Tile_t m_type;
	int m_origin; // upper left
	int m_config_id; // per objects.h
};

class Board_t {
public:
	Board_t(int maxrows, int maxcols);

	void initialize(const std::vector<int>& input);
	std::vector<Region_t> getAllRegions() const;
	Region_t getRegionAt(int row, int col) const;
	Region_t getBorder(Region_t region) const;
	
	Tile_t peek(int row, int col) const;
	Tile_t peek(int index) const;
	int getRow(int index) const;
	int getCol(int index) const;
	int getIndex(int row, int col) const;

	bool breakRegion(Region_t region);
	bool breakRegionAt(int row, int col);

	std::vector<ObjectLoc_t> possibleMonokubLocs(Region_t region) const;
	std::vector<ObjectLoc_t> possibleHiddenMonokubLocs() const;

	std::vector<ObjectLoc_t> possibleFishieLocs(Region_t region) const;
	std::vector<ObjectLoc_t> possibleHiddenFishieLocs() const;

	void modify(int row, int col, Tile_t newType);

	void print();

private:
	std::vector<Tile_t> m_array;
	int m_rows;
	int m_cols;
};