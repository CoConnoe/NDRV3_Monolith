#include "stdafx.h"
#include "board.h"

int main() {
	Board_t board(3, 3);
	board.initialize({ 1, 1, 2, 2, 2, 1, 1, 1, 1 });
	std::vector<Region_t> regions = board.getAllRegions();
	
	return 0;
}