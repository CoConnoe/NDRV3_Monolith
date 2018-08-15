#include "stdafx.h"
#include "board.h"
#include <iostream>

void breakCommand(Board_t& board) {
	std::cout << "Row: ";
	int row;
	std::cin >> row;
	
	std::cout << "Col: ";
	int col;
	std::cin >> col;

	std::cout << std::endl;

	board.breakRegionAt(row, col);
}

int main() {
	Board_t board(3, 3);
	board.initialize({ 1, 1, 2, 2, 2, 3, 1, 1, 3 });
	std::vector<Region_t> regions = board.getAllRegions();
	
	while (1) {
		board.print();
		std::cout << std::endl;

		if (board.getAllRegions().empty()) {
			std::cout << "SOLVED" << std::endl;
			std::cout << "Score: " << std::endl; //TODO 
			return 0;
		}
		std::cout << "[B]reak [Q]uit" << std::endl;
		std::cout << "Command: ";

		char input;
		std::cin >> input;

		switch (input) {
		case 'B':
		case 'b':
			breakCommand(board);
			break;
		case 'Q':
		case 'q':
			return 0;
		}
	}
	
	
	return 0;
}