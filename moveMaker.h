#ifndef __moveMaker__
#define __moveMaker__

#include "Board.h"
#include "defs.h"
#include <iostream>

using namespace std;


class moveMaker
{
public:
	moveMaker();
	~moveMaker();

	void makeMove(const int move, Board& board);
	void takeBack(Board& board); // take move back and restore board to previous move state.
	void moveFwd(Board& board); // move forward after taking back a move
	void makeNullMove(Board& board); // make a null move, used in null move forward pruning.
	void takeNullMove(Board& board); // take back a null move.


private:
	void printMoveDetails(int move);
	void storeHist( const int move, Board& board); // store the details in the hist vector
	void setBitboards( const int move, Board& board); //set up bitboards from move int
	void remakeBitboards( const s_hist& mv_struct, Board& board); // set up bitboards and other data after a take back command
	void movePiece(const int piece, const int from, const int to, Board& board);
	void removePiece(const int piece, const int position, Board& board); // remove a piece from a given position
	void setPiece(const int piece, const int position, Board& board); // set a piece at a given position.
	int  formatNullMove(int from, int to, int cptPc, int promoPc ,bool promote, bool epCapt, bool pawnStrt, bool cstl); // create an int dummy move
    s_hist nullDetails;
};

#endif