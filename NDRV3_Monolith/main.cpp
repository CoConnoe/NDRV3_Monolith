#include "stdafx.h"
#include "board.h"
#include <iostream>
#include <stack>

void breakCommand(Board_t& board, std::stack<Board_t>& boardHistory, Region_t& bestRegion) {
	//std::cout << "Row: ";
	int row;
	std::cin >> row;
	
	//std::cout << "Col: ";
	int col;
	std::cin >> col;

	boardHistory.push(board);

	if (!board.breakRegionAt(row, col, true)) {
		boardHistory.pop();
	}

	bestRegion = Region_t();

	std::cout << std::endl;
}

void calcCommand(Board_t& board, std::stack<Board_t>& boardHistory, Region_t& bestRegion) {
	bestRegion = board.calculateBestMove();
}

void breakRecommendedCommaned(Board_t& board, std::stack<Board_t>& boardHistory, Region_t& bestRegion) {
	boardHistory.push(board);

	if (!board.breakRegion(bestRegion, true)) {
		boardHistory.pop();
	}

	bestRegion = Region_t();

	std::cout << std::endl;
}

void modifyCommand(Board_t& board, std::stack<Board_t>& boardHistory, Region_t& bestRegion) {
	//std::cout << "Row: ";
	int row;
	std::cin >> row;

	//std::cout << "Col: ";
	int col;
	std::cin >> col;

	//std::cout << "0-Empty 1-White 2-Pink 3-Orange 4-Blue M-Monokub F-Fishie" << std::endl;
	//std::cout << "Value: ";
	char val;
	std::cin >> val;

	Tile_t valtype;
	switch (val) {
	case '0':
		valtype = TILE_EMPTY;
		break;
	case '1':
		valtype = TILE_WHITE;
		break;
	case '2':
		valtype = TILE_PINK;
		break;
	case '3':
		valtype = TILE_ORANGE;
		break;
	case '4':
		valtype = TILE_BLUE;
		break;
	case 'M':
	case 'm':
		valtype = TILE_MONOKUB;
		break;
	case 'F':
	case 'f':
		valtype = TILE_FISHIE;
		break;
	default:
		valtype = NOT_TILE_OOB;
		break;
	}

	if (valtype == NOT_TILE_OOB) {
		std::cout << "ERROR: Invalid tile type" << std::endl;
	}
	else {
		boardHistory.push(board);

		if (!board.modify(row, col, valtype)) {
			boardHistory.pop();
		}
			
		bestRegion = Region_t();
	}

	std::cout << std::endl;
}

void undoCommand(Board_t& board, std::stack<Board_t>& boardHistory, Region_t& bestRegion) {
	if (boardHistory.empty()) {
		std::cout << "ERROR: No more history!" << std::endl;
	}
	else {
		board = boardHistory.top();
		boardHistory.pop();
		bestRegion = Region_t();
	}

	std::cout << std::endl;
}

int main() {
	Board_t board(7, 8);
	board.initialize({ 1, 4, 3, 4, 4, 4, 3, 3,
					   4, 1, 4, 3, 4, 3, 2, 2, 
					   2, 3, 1, 2, 2, 1, 3, 1,
		               1, 3, 2, 2, 1, 2, 4, 1,
		               4, 3, 4, 1, 2, 3, 4, 4,
					   4, 1, 4, 3, 4, 3, 4, 1,
		               2, 3, 1, 2, 1, 2, 2, 1});
	std::stack<Board_t> boardHistory;
	Region_t bestRegion;

	while (1) {
		board.print(bestRegion);
		std::cout << "Score: " << board.calculateScore() << std::endl;
		std::cout << std::endl;

		if (board.getAllRegions().empty()) {
			std::cout << "SOLVED" << std::endl;
			return 0;
		}
		std::cout << "[B]reak [C]alc [R]ecommended [M]odify [U]ndo [Q]uit" << std::endl;
		std::cout << "Command: ";

		char input;
		std::cin >> input;

		switch (input) {
		case 'B':
		case 'b':
			breakCommand(board, boardHistory, bestRegion);
			break;
		case 'C':
		case 'c':
			calcCommand(board, boardHistory, bestRegion);
			break;
		case 'R':
		case 'r':
			breakRecommendedCommaned(board, boardHistory, bestRegion);
			break;
		case 'M':
		case 'm':
			modifyCommand(board, boardHistory, bestRegion);
			break;
		case 'U':
		case 'u':
			undoCommand(board, boardHistory, bestRegion);
			break;
		case 'Q':
		case 'q':
			return 0;
		}
	}
	
	return 0;
}