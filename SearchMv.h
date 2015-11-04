#ifndef __search__
#define __search__
#include "MoveGen.h"
#include "Eval.h"

#define R 2 // null move pruning reduction factor.
class SearchMv
{
public:
	SearchMv();
	~SearchMv();
	int getBestMove(Board& board,s_searchParamas* params); // calculates the best move in the alloted time.
	int evaluate(Board& board);
	

private:

int alphaBeta(int depth,int alpha, int beta, Board& board, int mate, vector<int>& pLine, bool allowNull,s_searchParamas* params);
int quiescentSearch(int alpha, int beta, Board& board,s_searchParamas* params);
bool isZugzwangChance(Board& board); // is there a possibility of a zugzwang position, used to avoid a null move forward pruning.
vector<int> getMoves(Board& board); // get a vector with the moves in the best possible ordering.
vector<int> PVLine; // store the PV line here.
int prevBestMv; // the previously best move.
moveMaker myMaker;
bool isRepetion(Board& board); // check if a move is a repetion.
bool isInsuffcientMaterial(Board& board); // check if insufficient material to mate (draw)
void statusCheck(s_searchParamas* params);// check every 2048 node counts if time is up or interupt message came from GUI

};


#endif


