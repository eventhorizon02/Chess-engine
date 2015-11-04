#ifndef __Eval__
#define __Eval__

#include "Board.h"
#include "MoveGen.h"
#include <fstream>
#include <string>




class Eval
{
public:
	Eval(Board* board);
	~Eval(){};

	bool isCheckmate(Board* board); // are we in a checkmate situation?
	int evalBoard(Board* board); // return a score for the board.
	void mirrorTest(Board* board); // run the eval then run it again on a mirrored board and compare the score, make sure they are identical.
	bool isChanceZugzwang(Board* board); // is there a chance of a zugzwang style position?, to be used to avoid a null move forward pruning.
	

private:
	int calculateMaterialBalance(Board* board); // the score based purley on material balace.
	int claculatePositionValues(Board* board); // the adjusmets for positional control
	int calcPawnsPosVal(Board* board); // adjustments for pawn positions.
	int calcKnightsPosVal(Board* board);// adjusments for knight positions.
	int calcBishopPosVal(Board* board);// adjusments for bishop positions.
	int calcKingPosVal(Board* board);// adjusments for king positions.
	int calcPassedPawnsVal(Board* board); // give bunos for passed pawns.
	int calcIsoPawnsVal(Board* board); // apply penalty for isolated pawns and double pawns
	int rooksAndQueensVal(Board* board); // give bunos for rooks and queens on open files, semi open files, and rook on seventh rank.
	bool isFileOpen(Board* board, int file); // are there no pawns on a given file, both colors
	bool isSemiOpen(Board* board, int file, int color); // are there no pawns of same color on a given file
	bool isEndGame(Board* board); // are we in the end game phase?
	
};



#endif
















