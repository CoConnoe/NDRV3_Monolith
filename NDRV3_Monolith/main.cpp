#include "stdafx.h"
#include "board.h"
#include <iostream>
#include <stack>

void breakCommand(Board_t& board, std::stack<Board_t>& boardHistory) {
	std::cout << "Row: ";
	int row;
	std::cin >> row;
	
	std::cout << "Col: ";
	int col;
	std::cin >> col;

	boardHistory.push(board);

	if (!board.breakRegionAt(row, col))
		boardHistory.pop();

	std::cout << std::endl;
}

void undoCommand(Board_t& board, std::stack<Board_t>& boardHistory) {
	if (boardHistory.empty()) {
		std::cout << "ERROR: No more history!" << std::endl;
	}
	else {
		board = boardHistory.top();
		boardHistory.pop();
	}

	std::cout << std::endl;
}

int main() {
	Board_t board(3, 3);
	board.initialize({ 1, 1, 2, 2, 2, 3, 1, 1, 3 });
	std::stack<Board_t> boardHistory;

	while (1) {
		board.print();
		std::cout << std::endl;

		if (board.getAllRegions().empty()) {
			std::cout << "SOLVED" << std::endl;
			std::cout << "Score: " << std::endl; //TODO 
			return 0;
		}
		std::cout << "[B]reak [U]ndo [Q]uit" << std::endl;
		std::cout << "Command: ";

		char input;
		std::cin >> input;

		switch (input) {
		case 'B':
		case 'b':
			breakCommand(board, boardHistory);
			break;
		case 'U':
		case 'u':
			undoCommand(board, boardHistory);
			break;
		case 'Q':
		case 'q':
			return 0;
		}
	}
	
	return 0;
}