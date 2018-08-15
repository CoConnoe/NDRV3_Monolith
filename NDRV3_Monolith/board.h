#pragma once

#include <random>
#include <set>
#include <vector>
#include "objects.h"

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
	int m_origin; // upper left
	int m_config_id; // per objects.h
};

class Board_t {
public:
	Board_t(int maxrows, int maxcols);

	void initialize(const std::vector<int>& input);

	bool breakRegion(const Region_t& region);
	bool breakRegionAt(int row, int col);
	bool modify(int row, int col, Tile_t newType);

	std::vector<Region_t> getAllRegions() const;
	Region_t calculateBestMove() const;
	int calculateScore() const;
	void print(const Region_t& bestRegion) const;

protected:

	Region_t getRegionAt(int row, int col) const;
	Region_t getBorder(const Region_t& region) const;
	Tile_t peek(int row, int col) const;
	Tile_t peek(int index) const;
	int getRow(int index) const;
	int getCol(int index) const;
	int getIndex(int row, int col) const;

	int calculateOptimalScore(const std::vector<ObjectLoc_t>& monokubLocs, const std::vector<ObjectLoc_t>& fishieLocs, int depth);
	int mashButtonsScore(const std::vector<ObjectLoc_t>& monokubLocs, const std::vector<ObjectLoc_t>& fishieLocs, std::default_random_engine& generator, int depth);

	std::vector<ObjectLoc_t> possibleMonokubLocs(const Region_t& region) const;
	std::vector<ObjectLoc_t> possibleHiddenMonokubLocs() const;

	std::vector<ObjectLoc_t> possibleFishieLocs(const Region_t& region) const;
	std::vector<ObjectLoc_t> possibleHiddenFishieLocs() const;

	std::vector<ObjectLoc_t> possibleObjectLocs(const Region_t& region, const std::vector<Object_t>& allObjects) const;

	void applyObjectLocations(const std::vector<ObjectLoc_t>& monokubLocs, const std::vector<ObjectLoc_t>& fishieLocs);

	bool couldBeHiddenHere(const Object_t& object, int originIndex) const;
	bool isCompletelyRevealed(const Object_t& object, int originIndex) const;

private:
	std::vector<Tile_t> m_array;
	int m_rows;
	int m_cols;
};