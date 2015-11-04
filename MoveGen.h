#ifndef __MoveGen__
#define __MoveGen__
#include "Board.h"
#include "moveMaker.h"
#include <vector>
#include <algorithm>


using namespace std;

struct move_s
{
	move_s(){};
	move_s(int myMove, int myScore)
	{
		move = myMove;
		score = myScore;
	};
	int move;
	int score;
	
};

class MoveGen
{
public:
	MoveGen(Board* board);
	MoveGen(Board* board, bool capsOnly);// constructor to generate only the capturing moves.
	~MoveGen();
	int formatMove(int from, int to, int cptPc, int promoPc, bool promote, bool epCapt, bool pawnStrt, bool cstl);
	vector<int> moves_v; // contains all legal moves
    vector<int> caps_v; // contains all capture moves
	int num_legal_moves;


private:
	vector<move_s> moves_m;
	void generateAllMoves(Board *board); // generate and store all the moves for a given side per board position.
	void generateAllCaps(Board* board); // generate and store all the capture moves.
	void generatePawnMoves(const Board* board);
	void generateSlidersMoves(const Board* board);
	void generateNonSliderMoves(const Board* board);
	void generateCastleMoves(const Board* board);
	void generatePsuedoMoves(const Board* board); // generate all possible moves including the ones that put or keep you in check
	void sortAndRemove(Board* board); // sort the moves in order of MVV/LVA and remove the moves that put self in check.
	// generate only cpaturing moves
	void generatePsuedoCaps(const Board* board);
    void generatePawnCaps(const Board* board); // generate pawn capturing moves.
	void generateSlidersCaps(const Board* board);// generate the sliders cpturing moves.
	void generateNonSliderCaps(const Board* board);// generate non slider caps moves.
	void removeIllegalCaps(Board* board);
	

};


#endif

