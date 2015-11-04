#include "SearchMv.h"


SearchMv::SearchMv()
{
  moveMaker maker;
  this->myMaker = maker;
  PVLine.clear();
  prevBestMv = NO_MOVE;
}

SearchMv::~SearchMv()
{

}

void SearchMv::statusCheck(s_searchParamas* params)
 {
	// check if time up, or interrupt from GUI
	if(params->timeset == true && returnMillisecs() > params->stoptime) 
	params->stopped = true;
	ReadInput(params);
}


int SearchMv::getBestMove(Board& board, s_searchParamas* params)
{
	 vector<int> line; // store a fresh PV line here.
    for (int depth = 1;depth <= params->depth; ++depth)
   {
   	  if (params->stopped)
      break;
   	
    line.clear();
    if (!PVLine.empty())
    prevBestMv = PVLine[0]; // store the best move so far in case the search gets interupted which will destroy it and return an empty line.
    int bestScore = alphaBeta(depth, -INF, INF, board, MATE, line, true,params);
    if (!line.empty())
    {
     PVLine = line; // transfer the fresh line to the class member PVLine so we can work with it in the get moves function.
     int cp = bestScore / 100; // the score in centipawns.
     cout << "info score cp " << cp << " depth " << depth << " nodes " << params->nodes << " time " << returnMillisecs() - params->starttime << " pv";
     for (vector<int>::iterator it = PVLine.begin(), end = PVLine.end(); it != end; ++it)
     cout << " " << returnAlgebraic(*it);
     cout << endl;
     }
  }

   if (!PVLine.empty())
       return PVLine[0];
   else if (prevBestMv != NO_MOVE)
   	   return prevBestMv; // expect to get here most times because search will be interupted by "time up" or interupt from GUI
   
       return NO_MOVE;

 }

// return a score for the current board position.
int SearchMv::evaluate(Board& board)
{
	Eval ev(&board);
	return ev.evalBoard(&board);
}

// is there a possibility of a zugzwang position, used to avoid a null move forward pruning.

bool SearchMv::isZugzwangChance(Board& board)
{ 
	Eval ev(&board);
	return ev.isChanceZugzwang(&board);
}



int SearchMv::alphaBeta(int depth,int alpha, int beta, Board& board, int mate, vector<int>& pLine, bool allowNull,s_searchParamas* params)
{
	if ( (params->nodes & 2047) == 0) // go read input every 2048 node count.
		statusCheck(params);
	params->nodes++;
	 int val;
	 bool foundPV = false;
	 vector<int> line;

	  if (depth <= 0)
      //return evaluate(board);
     return quiescentSearch(alpha, beta, board,params);


      // null move forward prunning, will test more later
      /*
      if ( (depth >= 3) && (allowNull) && (board.isInCheck(board.turn) == false) && (isZugzwangChance(board) == false) )
      {
           myMaker.makeNullMove(board);
           val = -alphaBeta(depth - 1 - R, -beta, -beta + 1, board, mate - 1, line, false,params);
           myMaker.takeNullMove(board);
           if (val >= beta)
           return beta;
      }
      */
     
      vector<int> myMoves = getMoves(board);
      int numMoves = myMoves.size();
      if (board.isInCheck(board.turn))
      {
      	if (numMoves == 0)
      		return -mate;
      	else depth++; // in check extention
      }else
      {
         if (numMoves == 0) // draw
         	return 0; 
      }
       
       for (vector<int>::iterator it = myMoves.begin(), end = myMoves.end(); it != end; ++it)
     {
     	myMaker.makeMove(*it, board);
      if (isRepetion(board) || (board.hm_clock >= 50) || isInsuffcientMaterial(board))
      {
        myMaker.takeBack(board);
        return 0;
      }
     
        if (foundPV)
        {
        	val = -alphaBeta(depth - 1, -alpha - 1, -alpha, board ,mate - 1, line, true,params);
        	if ((val > alpha) && (val < beta)) // Check for failure.
        	 val = -alphaBeta(depth - 1,-beta,-alpha, board, mate - 1, line, true,params);
         }else
        val = -alphaBeta(depth - 1,-beta,-alpha, board, mate - 1, line, true,params);
        
     	myMaker.takeBack(board);

      if(params->stopped) 
      return 0;

     	if (val >= beta)
            return beta;
        if (val > alpha)
        {
            alpha = val;
            foundPV = true;
            pLine = line;
            pLine.insert(pLine.begin(), *it);
        }
      } 
     
       return alpha;
}
// search only the capture moves, resolve all the capture moves to help mitigate the horizon effect.
int SearchMv::quiescentSearch(int alpha, int beta, Board& board,s_searchParamas* params)
{
	if ( (params->nodes & 2047) == 0) // go read input every 2048 node count.
		statusCheck(params);
	params->nodes++;
	int val = evaluate(board);
	if (val >= beta)
        return beta;
    if (val > alpha)
        alpha = val;

    MoveGen gen(&board, true); // caps moves only constructor
    for (vector<int>::iterator it = gen.caps_v.begin(), end = gen.caps_v.end(); it != end; ++it)
	{
		myMaker.makeMove(*it, board);
		val = -quiescentSearch(-beta, -alpha, board,params);
		myMaker.takeBack(board);
    if(params->stopped) 
      return 0;
      
		if (val >= beta)
            return beta;
        if (val > alpha)
            alpha = val;
    }

    return alpha;
}

// get a vector with the moves in the best possible ordering.
vector<int> SearchMv::getMoves(Board& board)
{
	MoveGen gen(&board);
	if (!PVLine.empty())
	{
		vector<int>::iterator itNext = PVLine.begin();
		int nextPvMove = *itNext;
		PVLine.erase(PVLine.begin());
		for (vector<int>::iterator it = gen.moves_v.begin(), end = gen.moves_v.end(); it != end; ++it)
		{
			if (nextPvMove == *it)
			{
				gen.moves_v.erase(it);
				gen.moves_v.insert(gen.moves_v.begin(), nextPvMove);
				break;
			}
		}
	}

     return gen.moves_v;
}

// check if a move is a repetion.
bool SearchMv::isRepetion(Board& board)
{
  
   for(vector<s_hist>::iterator it = board.board_hist.end() - board.hm_clock, end = board.board_hist.end(); it != end; ++ it)
   {
      s_hist s_temp = *it;
     if (s_temp.hashKey == board.hashKey)
     return true;
   }
   return false;
}

// check if insufficient material to mate (draw)
bool SearchMv::isInsuffcientMaterial(Board& board)
{
   if ( (!board.pList[WP].empty()) || (!board.pList[BP].empty()) ) return false;
   if ( (!board.pList[WQ].empty()) || (!board.pList[BQ].empty()) || (!board.pList[WR].empty()) || (!board.pList[BR].empty()) ) return false;
   if ( (board.pList[WB].size() > 1) || (board.pList[BB].size() > 1) ) return false;
   if ( (!board.pList[WB].empty()) && (!board.pList[WN].empty()) ) return false;
   if ( (!board.pList[BB].empty()) && (!board.pList[BN].empty()) ) return false;
   
   return true;
}











