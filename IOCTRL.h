#ifndef __IOCTRL__
#define __IOCTRL__

#include <iostream>
#include <string>
#include "Board.h"
#include "defs.h"
#include "moveMaker.h"
#include "MoveGen.h"
#include "SearchMv.h"

#define PRINT_BOARD -5
#define TAKE_BACK -10
#define MOVE_FORWARD -15
#define QUIT -20
#define MIRROR_BOARD -25
#define MOVE_MADE -30
#define INPUTBUFFER 400 * 6


class IOCTRL
{
public:
	IOCTRL();
   ~IOCTRL(){};
   int makeMove(Board& board);
   int calculate(Board& board);
   void UCILoop(); // the main loop while playing with GUI in UCI protocol


private:
	int returnMoveInt(string inputString, vector<int>& moves_v); // return the proper move int from the list of all legal moves
	void ParseGo(char* line, s_searchParamas *params, Board* board);
	void ParsePosition(char* lineIn, Board* board);
	void searchPos(Board& board, s_searchParamas *params);
    moveMaker maker;
	
};



#endif
