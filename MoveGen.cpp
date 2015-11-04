#include "MoveGen.h"


MoveGen::MoveGen(Board* board)
{
   num_legal_moves = -1;
   generateAllMoves(board);
   num_legal_moves = moves_v.size();
   assert(moves_v.size() != -1);
}

// generate and store only the caps moves

MoveGen::MoveGen(Board* board, bool capsOnly)
{
  
   generateAllCaps(board);
   
}

MoveGen::~MoveGen()
{

}

int MoveGen::formatMove(int from, int to, int cptPc, int promoPc ,bool promote, bool epCapt, bool pawnStrt, bool cstl)
{
	int move = 0;
    move |= to; // first 7 bits of the to are set.
    move |= ( from << 7); // next 7 bits are set with the from square.

    // set the captured piece bits
    move |= (cptPc << 14);

    // set the promoted to piece
    move |= (promoPc << 18);

    // set the flags
    move |= (promote << 22);
    move |= (epCapt << 23);
    move |= (pawnStrt << 24);
    move |= (cstl << 25);

    return move;
}

// generate and store all the moves for a given side per board position.

void MoveGen::generateAllMoves(Board *board)
{
    moves_v.clear();
    moves_m.clear();
    generatePsuedoMoves(board);
    sortAndRemove(board);
} 

void MoveGen::generateAllCaps(Board* board)
{
	caps_v.clear();
	generatePsuedoCaps(board);
	removeIllegalCaps(board);
}



// generate all possible moves including the ones that put or keep you in check       
void MoveGen::generatePsuedoMoves(const Board* board)
{
    generateNonSliderMoves(board);
    generatePawnMoves(board);
    generateSlidersMoves(board);
    generateCastleMoves (board);
}

// generate all possible capture moves including the ones that put or keep you in check
void MoveGen::generatePsuedoCaps(const Board* board)
{
   generatePawnCaps(board);
   generateSlidersCaps(board);
   generateNonSliderCaps(board);
}
 

// generate all legal pawn moves

void MoveGen::generatePawnMoves(const Board* board)
{
  int side = board->turn;
	if (side == WHITE)
	{
		vector<int> pawn_list = board -> pList[WP];
		for (vector<int>::iterator it = pawn_list.begin(), end = pawn_list.end(); it != end; ++it)
		{
			int targetSq = *it + 10;
			if ( (board ->_board[targetSq] == EMPTY) && (ranks[targetSq] < 8) && (ranks[targetSq] != -1) )
				moves_m.push_back(move_s(formatMove(*it, targetSq, EMPTY, EMPTY, false, false, false, false), 50));

			if ( (ranks[*it] == 2) && ( board -> _board[*it + 20] == EMPTY) && ( board -> _board[*it + 10] == EMPTY) ) // two squares pawn start
				moves_m.push_back(move_s(formatMove(*it, *it + 20, EMPTY, EMPTY, false, false, true, false), 50));

			//check for capture moves (non promotion captures, non en-passant captures)
			int left_target = *it + 11;
			int right_target = *it + 9;

			if ( (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (ranks[left_target] < 8) && (board ->_color[sq120to64[left_target]] == BLACK))
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], EMPTY, false, false, false, false), board->mvvlva[board ->_board[left_target]][WP]));

			if ( (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (ranks[right_target] < 8) && (board ->_color[sq120to64[right_target]] == BLACK))
			    moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], EMPTY, false, false, false, false), board->mvvlva[board ->_board[right_target]][WP]));

			// check for en-passant captures
			if ( (ranks[board ->enPasSq] == 6) && (board ->_board[left_target] == EMPTY) && (board -> enPasSq == left_target))
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[*it + 1], EMPTY, false, true, false, false), board->mvvlva[board ->_board[*it + 1]][WP]));

			if ( (ranks[board ->enPasSq] == 6) && (board ->_board[right_target] == EMPTY) && (board -> enPasSq == right_target))
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[*it -1], EMPTY, false, true, false, false), board->mvvlva[board ->_board[*it -1]][WP]));


            // check for captures promotions
			if ( (ranks[left_target] == 8) && (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (board ->_color[sq120to64[left_target]] == BLACK))
			{
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], WQ, true, false, false, false),board->mvvlva[board ->_board[left_target]][WP]));
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], WR, true, false, false, false),board->mvvlva[board ->_board[left_target]][WP]));
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], WB, true, false, false, false),board->mvvlva[board ->_board[left_target]][WP]));
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], WN, true, false, false, false),board->mvvlva[board ->_board[left_target]][WP]));
			}

			if ( (ranks[right_target] == 8) && (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (board ->_color[sq120to64[right_target]] == BLACK))
			{
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], WQ, true, false, false, false),board->mvvlva[board ->_board[right_target]][WP]));
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], WR, true, false, false, false),board->mvvlva[board ->_board[right_target]][WP]));
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], WB, true, false, false, false),board->mvvlva[board ->_board[right_target]][WP]));
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], WN, true, false, false, false),board->mvvlva[board ->_board[right_target]][WP]));
			}

            // check for straight in promotions
            if ( (ranks[*it + 10] == 8) && (board ->_board[*it + 10] == EMPTY))
            {
            	moves_m.push_back(move_s(formatMove(*it, *it + 10, EMPTY, WQ, true, false, false, false),50));
            	moves_m.push_back(move_s(formatMove(*it, *it + 10, EMPTY, WR, true, false, false, false),50));
            	moves_m.push_back(move_s(formatMove(*it, *it + 10, EMPTY, WB, true, false, false, false),50));
                moves_m.push_back(move_s(formatMove(*it, *it + 10, EMPTY, WN, true, false, false, false),50));
            }
       }
	}else{

		vector<int> pawn_list = board -> pList[BP];
		for (vector<int>::iterator it = pawn_list.begin(), end = pawn_list.end(); it != end; ++it)
		{
			int targetSq = *it - 10;
			if ( (board ->_board[targetSq] == EMPTY) && (ranks[targetSq] > 1) && (ranks[targetSq] != -1) )
				moves_m.push_back(move_s(formatMove(*it, targetSq, EMPTY, EMPTY, false, false, false, false),50));

			if ( (ranks[*it] == 7) && ( board -> _board[*it - 20] == EMPTY) && ( board -> _board[*it - 10] == EMPTY)) // two squares pawn start
				moves_m.push_back(move_s(formatMove(*it, *it - 20, EMPTY, EMPTY, false, false, true, false),50));

			//check for capture moves (non promotion captures, non en-passant captures)
			int left_target = *it - 11;
			int right_target = *it - 9;

			if ( (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (ranks[left_target] > 1) && (board ->_color[sq120to64[left_target]] == WHITE))
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], EMPTY, false, false, false, false),board->mvvlva[board ->_board[left_target]][BP]));

			if ( (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (ranks[right_target] > 1) && (board ->_color[sq120to64[right_target]] == WHITE))
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], EMPTY, false, false, false, false),board->mvvlva[board ->_board[right_target]][BP]));

			// check for en-passant captures
			if ( (ranks[board ->enPasSq] == 3) && (board ->_board[left_target] == EMPTY) && (board -> enPasSq == left_target))
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[*it - 1], EMPTY, false, true, false, false),board->mvvlva[board ->_board[*it - 1]][BP]));

			if ( (ranks[board ->enPasSq] == 3) && (board ->_board[right_target] == EMPTY) && (board -> enPasSq == right_target))
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[*it + 1], EMPTY, false, true, false, false),board->mvvlva[board ->_board[*it + 1]][BP]));

			// check for captures promotions
			if ( (ranks[left_target] == 1) && (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (board ->_color[sq120to64[left_target]] == WHITE))
			{
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], BQ, true, false, false, false),board->mvvlva[board ->_board[left_target]][BP]));
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], BR, true, false, false, false),board->mvvlva[board ->_board[left_target]][BP]));
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], BB, true, false, false, false),board->mvvlva[board ->_board[left_target]][BP]));
				moves_m.push_back(move_s(formatMove(*it, left_target, board ->_board[left_target], BN, true, false, false, false),board->mvvlva[board ->_board[left_target]][BP]));
			}

			if ((ranks[right_target] == 1) && (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (board ->_color[sq120to64[right_target]] == WHITE))
			{
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], BQ, true, false, false, false),board->mvvlva[board ->_board[right_target]][BP]));
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], BR, true, false, false, false),board->mvvlva[board ->_board[right_target]][BP]));
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], BB, true, false, false, false),board->mvvlva[board ->_board[right_target]][BP]));
				moves_m.push_back(move_s(formatMove(*it, right_target, board ->_board[right_target], BN, true, false, false, false),board->mvvlva[board ->_board[right_target]][BP]));
			}

			// check for straight in promotions
            if ( (ranks[*it - 10] == 1) && (board ->_board[*it - 10] == EMPTY))
            {
            	moves_m.push_back(move_s(formatMove(*it, *it - 10, EMPTY, BQ, true, false, false, false),50));
            	moves_m.push_back(move_s(formatMove(*it, *it - 10, EMPTY, BR, true, false, false, false),50));
            	moves_m.push_back(move_s(formatMove(*it, *it - 10, EMPTY, BB, true, false, false, false),50));
            	moves_m.push_back(move_s(formatMove(*it, *it - 10, EMPTY, BN, true, false, false, false),50));
            }

		}
    }

}

// generate silders moves (queens, bishops, rooks)

void MoveGen::generateSlidersMoves(const Board* board)
{
  int side = board->turn;
	int start,finish; // where to start looping for the pieces.
    if (side == WHITE){
    	start = 0;
    	finish = 5;
    }else{
    	start = 6;
    	finish = 11;
    }

    for (int pcs = start, indx = 0; pcs <= finish; ++pcs, ++indx)
    {
       if (isSlider[indx]) // we found a vector of a slider type
       {
          vector<int> v = board -> pList[pcs];
          for (vector<int>::iterator it = v.begin(), end = v.end(); it != end; ++it)
          {
          	int direc;
          	for (int i = 0; i < 8; ++i)
          	{
          		direc = pieceVectors[indx][i];
          		int testSq = *it + direc;
          		while (board ->_board[ testSq ] != OFFBOARD)
          		{
          			if ( board ->_board[testSq] != EMPTY) 
          			{
          				if (board ->_color[sq120to64[testSq]] == (side ^ 1)) // we've encoutered a piece from the enemy color)
                         moves_m.push_back(move_s(formatMove(*it, testSq, board ->_board[testSq], EMPTY, false, false, false, false),board->mvvlva[board ->_board[testSq]][pcs]));
                         break; // or we have encountered a friendly piece if the above if fails, either way we are done iterating in that direction.
          			} 

          		    moves_m.push_back(move_s(formatMove(*it, testSq, EMPTY, EMPTY, false, false, false, false),50));
                   testSq += direc;
          		}
          	}
          }
       }
    }
}

// generate non slider moves (kings and knights)
void MoveGen::generateNonSliderMoves(const Board* board)
{
  int side = board->turn;
	int tmp_piece;
    vector<int> knight_list = (side == WHITE) ? board -> pList[WN] : board -> pList[BN];
        for ( vector<int>::iterator it = knight_list.begin(),end = knight_list.end(); it != end; ++it )
        {
           int sq = *it;
           for (int index = 0; index < 8; index++)
           {
              tmp_piece = board -> _board[sq + pieceVectors[KNIGHT][index]];
              if ( (tmp_piece != OFFBOARD) && ( tmp_piece == EMPTY) )
              	moves_m.push_back(move_s(formatMove(sq,sq + pieceVectors[KNIGHT][index], EMPTY, EMPTY, false, false, false, false),50));

              if ( (tmp_piece != OFFBOARD) && ( board ->_color[sq120to64[sq + pieceVectors[KNIGHT][index]]] == (side ^ 1) ))
                moves_m.push_back(move_s(formatMove(sq,sq + pieceVectors[KNIGHT][index], tmp_piece, EMPTY, false, false, false, false),board->mvvlva[tmp_piece][(side == WHITE) ? WN : BN]));
           }
        }

      // add the kings moves
      int kings_pos = (side == WHITE) ? board -> pList[WK][0]: board ->pList[BK][0];
      for (int index = 0; index < 8; ++index)
      {
      	 int tmp_sq = board ->_board[kings_pos + pieceVectors[KING][index]];
      	if ( (tmp_sq != OFFBOARD) && (tmp_sq == EMPTY) )
      		moves_m.push_back(move_s(formatMove(kings_pos, kings_pos + pieceVectors[KING][index], EMPTY, EMPTY, false, false, false, false),50));

      	if ( (tmp_sq != OFFBOARD) && ( board ->_color[sq120to64[ kings_pos + pieceVectors[KING][index]]] == (side ^ 1) ))
      		moves_m.push_back(move_s(formatMove(kings_pos, kings_pos + pieceVectors[KING][index], tmp_sq, EMPTY, false, false, false, false),board->mvvlva[tmp_sq][(side == WHITE) ? WK : BK]));

      }
}

// generate the castle moves

void MoveGen::generateCastleMoves(const Board* board)
{
   int side = board->turn;
  if ( !board -> isInCheck(side))
{
   if (side == WHITE)
   {
	 if ( (board -> extractCastRgt (WKC) ) && ( !board -> isSqAttacked(F1, BLACK) ) &&  ( board -> _board[F1] == EMPTY) &&(board -> _board[G1] == EMPTY))
	   {
          assert(board -> _board[E1] == WK);
          assert(board -> _board[H1] == WR);
          moves_m.push_back(move_s(formatMove(E1, G1, EMPTY, EMPTY, false, false, false, true),50));
	   }

	   if ( (board -> extractCastRgt (WQC) ) && ( !board -> isSqAttacked(D1, BLACK) ) && ( board -> _board[C1] == EMPTY) &&(board -> _board[D1] == EMPTY)&&(board -> _board[B1] == EMPTY))
	   {
	   	 assert(board -> _board[E1] == WK);
         assert(board -> _board[A1] == WR);
         moves_m.push_back(move_s(formatMove(E1, C1, EMPTY, EMPTY, false, false, false, true),50));
	   }
	}else
	{
		if ( (board -> extractCastRgt (BKC) ) && ( !board -> isSqAttacked(F8, WHITE) ) && ( board -> _board[F8] == EMPTY) &&(board -> _board[G8] == EMPTY))
	   {
          assert(board -> _board[E8] == BK);
          assert(board -> _board[H8] == BR);
          moves_m.push_back(move_s(formatMove(E8, G8, EMPTY, EMPTY, false, false, false, true),50));
	   }

	   if ( (board -> extractCastRgt (BQC) ) && ( !board -> isSqAttacked(D8, WHITE) ) && ( board -> _board[C8] == EMPTY) &&(board -> _board[D8] == EMPTY)&&(board -> _board[B8] == EMPTY))
	   {
	   	 assert(board -> _board[E8] == BK);
         assert(board -> _board[A8] == BR);
         moves_m.push_back(move_s(formatMove(E8, C8, EMPTY, EMPTY, false, false, false, true),50));
	   }

	}

}
	
}
//a function to be sent as a paramenet to the sort function.
bool sortByScore(const move_s &lhs, const move_s &rhs)
{
   return lhs.score > rhs.score;
}

// sort the moves in order of MVV/LVA and remove the moves that put self in check.
void MoveGen::sortAndRemove(Board* board)
{
    moveMaker maker;
	sort(moves_m.begin(), moves_m.end(), sortByScore);
	for (vector<move_s>::iterator it = moves_m.begin(), end = moves_m.end(); it != end; ++it)
	{
       maker.makeMove(it->move,*board);
       if (! board->isInCheck( board->turn ^ 1 ) ) // side switched after making the move so we have to check for other side.
       moves_v.push_back(it->move);
       maker.takeBack(*board);
    }

}

//********************* generate only the capture moves ***********************************

void MoveGen::generatePawnCaps(const Board* board)
{    
	int side = board->turn;
	if (side == WHITE)
	{
		vector<int> pawn_list = board -> pList[WP];
		for (vector<int>::iterator it = pawn_list.begin(), end = pawn_list.end(); it != end; ++it)
		{
			//check for capture moves (non promotion captures, non en-passant captures)
			int left_target = *it + 11;
			int right_target = *it + 9;

			if ( (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (ranks[left_target] < 8) && (board ->_color[sq120to64[left_target]] == BLACK))
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], EMPTY, false, false, false, false));

			if ( (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (ranks[right_target] < 8) && (board ->_color[sq120to64[right_target]] == BLACK))
			    caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], EMPTY, false, false, false, false));

			// check for en-passant captures
			if ( (ranks[board ->enPasSq] == 6) && (board ->_board[left_target] == EMPTY) && (board -> enPasSq == left_target))
				caps_v.push_back(formatMove(*it, left_target, board ->_board[*it + 1], EMPTY, false, true, false, false));

			if ( (ranks[board ->enPasSq] == 6) && (board ->_board[right_target] == EMPTY) && (board -> enPasSq == right_target))
				caps_v.push_back(formatMove(*it, right_target, board ->_board[*it -1], EMPTY, false, true, false, false));


            // check for captures promotions
			if ( (ranks[left_target] == 8) && (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (board ->_color[sq120to64[left_target]] == BLACK))
			{
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], WQ, true, false, false, false));
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], WR, true, false, false, false));
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], WB, true, false, false, false));
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], WN, true, false, false, false));
			}

			if ( (ranks[right_target] == 8) && (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (board ->_color[sq120to64[right_target]] == BLACK))
			{
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], WQ, true, false, false, false));
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], WR, true, false, false, false));
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], WB, true, false, false, false));
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], WN, true, false, false, false));
			}
		}

        }else{

		vector<int> pawn_list = board -> pList[BP];
		for (vector<int>::iterator it = pawn_list.begin(), end = pawn_list.end(); it != end; ++it)
		{
		    //check for capture moves (non promotion captures, non en-passant captures)
			int left_target = *it - 11;
			int right_target = *it - 9;

			if ( (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (ranks[left_target] > 1) && (board ->_color[sq120to64[left_target]] == WHITE))
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], EMPTY, false, false, false, false));

			if ( (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (ranks[right_target] > 1) && (board ->_color[sq120to64[right_target]] == WHITE))
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], EMPTY, false, false, false, false));

			// check for en-passant captures
			if ( (ranks[board ->enPasSq] == 3) && (board ->_board[left_target] == EMPTY) && (board -> enPasSq == left_target))
				caps_v.push_back(formatMove(*it, left_target, board ->_board[*it - 1], EMPTY, false, true, false, false));

			if ( (ranks[board ->enPasSq] == 3) && (board ->_board[right_target] == EMPTY) && (board -> enPasSq == right_target))
				caps_v.push_back(formatMove(*it, right_target, board ->_board[*it + 1], EMPTY, false, true, false, false));

			// check for captures promotions
			if ( (ranks[left_target] == 1) && (board ->_board[left_target] != EMPTY) && (board ->_board[left_target] != OFFBOARD) && (board ->_color[sq120to64[left_target]] == WHITE))
			{
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], BQ, true, false, false, false));
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], BR, true, false, false, false));
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], BB, true, false, false, false));
				caps_v.push_back(formatMove(*it, left_target, board ->_board[left_target], BN, true, false, false, false));
			}

			if ((ranks[right_target] == 1) && (board ->_board[right_target] != EMPTY) && (board ->_board[right_target] != OFFBOARD) && (board ->_color[sq120to64[right_target]] == WHITE))
			{
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], BQ, true, false, false, false));
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], BR, true, false, false, false));
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], BB, true, false, false, false));
				caps_v.push_back(formatMove(*it, right_target, board ->_board[right_target], BN, true, false, false, false));
			}

		}
    }
    
}

void MoveGen::generateSlidersCaps(const Board* board)
{
	int side = board->turn;
	int start,finish; // where to start looping for the pieces.
    if (side == WHITE){
    	start = 0;
    	finish = 5;
    }else{
    	start = 6;
    	finish = 11;
    }

    for (int pcs = start, indx = 0; pcs <= finish; ++pcs, ++indx)
    {
       if (isSlider[indx]) // we found a vector of a slider type
       {
          vector<int> v = board -> pList[pcs];
          for (vector<int>::iterator it = v.begin(), end = v.end(); it != end; ++it)
          {
          	int direc;
          	for (int i = 0; i < 8; ++i)
          	{
          		direc = pieceVectors[indx][i];
          		int testSq = *it + direc;
          		while (board ->_board[ testSq ] != OFFBOARD)
          		{
          			if ( board ->_board[testSq] != EMPTY) 
          			{
          				if (board ->_color[sq120to64[testSq]] == (side ^ 1)) // we've encoutered a piece from the enemy color)
                         caps_v.push_back(formatMove(*it, testSq, board ->_board[testSq], EMPTY, false, false, false, false));
                         break; // or we have encountered a friendly piece if the above if fails, either way we are done iterating in that direction.
          			} 

          		    testSq += direc;
          		}
          	}
          }
       }
    }

}

void MoveGen::generateNonSliderCaps(const Board* board)
{
	int side = board->turn;
	int tmp_piece;
    vector<int> knight_list = (side == WHITE) ? board -> pList[WN] : board -> pList[BN];
        for ( vector<int>::iterator it = knight_list.begin(),end = knight_list.end(); it != end; ++it )
        {
           int sq = *it;
           for (int index = 0; index < 8; index++)
           {
              tmp_piece = board -> _board[sq + pieceVectors[KNIGHT][index]];
              if ( (tmp_piece != OFFBOARD) && ( board ->_color[sq120to64[sq + pieceVectors[KNIGHT][index]]] == (side ^ 1) ))
                caps_v.push_back(formatMove(sq,sq + pieceVectors[KNIGHT][index], tmp_piece, EMPTY, false, false, false, false));
           }
        }

      // add the kings moves
      int kings_pos = (side == WHITE) ? board -> pList[WK][0]: board ->pList[BK][0];
      for (int index = 0; index < 8; ++index)
      {
      	 int tmp_sq = board ->_board[kings_pos + pieceVectors[KING][index]];
      	if ( (tmp_sq != OFFBOARD) && ( board ->_color[sq120to64[ kings_pos + pieceVectors[KING][index]]] == (side ^ 1) ))
      		caps_v.push_back(formatMove(kings_pos, kings_pos + pieceVectors[KING][index], tmp_sq, EMPTY, false, false, false, false));
      }
}

// remove captures that puts self in check
void MoveGen::removeIllegalCaps(Board* board)
{
	moveMaker maker;
	for (vector<int>::iterator it = caps_v.begin(); it != caps_v.end();)
	{
       maker.makeMove(*it,*board);
       if ( board->isInCheck( board->turn ^ 1 ) ) // side switched after making the move so we have to check for other side.
       {
       	 it = caps_v.erase(it);
       }else
       ++it;
       maker.takeBack(*board);
    }
}

















