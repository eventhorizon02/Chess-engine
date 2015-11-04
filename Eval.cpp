#include "Eval.h"

#define isolated_pawn_penalty -15
#define doubled_pawn_penalty -20
#define rook_open_file 15
#define rook_semi_open_file 10
#define rook_on_seventh 20
#define queen_open_file 10
#define queen_semi_open_file 5

// passed pawns score values per rank
static const int pass_pawn_val[8] = { 0, 5, 10, 20, 40, 80, 200, 999 };
static const int bpass_pawn_val[8] ={ 999, 200, 80, 40, 20, 10, 5, 0 };




// piece position tables are points and penalties for pieces placed in different positions, they are based on whites side but
// can be used for black by applying the flip table.

const static int pawn_sq_table[64] = 
{
	0,  0,  0,   0,   0,  0,  0, 0,
	5, 10, 10, -25, -25, 10, 10, 5,
	5, -5,-10,   0,   0,-10, -5, 5,
	0,  0,  0,  25,  25,  0,  0, 0,
	5,  5, 10,  27,  27, 10,  5, 5,
	10,10, 20,  30,  30, 20, 10,10,
	50,50, 50,  50,  50, 50, 50,50,
	0,  0,  0,   0,   0,  0,  0, 0,
};

const static int knight_sq_table[64] = 
{
	-50, -30, -10, -10, -10, -10, -30, -50,
	-20,   0,   0,   0,   0,   0,   0, -20,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-20,   0,   0,   0,   0,   0,   0, -20,
	-50, -30, -10, -10, -10, -10, -30, -50

};

const static int bishop_sq_table[64] = 
{
   -5,-10,-10,-10,-10,-10,-10, -5,
  -10,  5,  0,  0,  0,  0,  5,-10,
  -10, 10, 10, 10, 10, 10, 10,-10,
  -10,  0, 10, 10, 10, 10,  0,-10,
  -10,  5,  5, 10, 10,  5,  5,-10,
  -10,  0,  5, 10, 10,  5,  0,-10,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -20,-10,-10,-10,-10,-10,-10,-20,
};

const static int king_sq_table[64] =
{
	  0,  20,  40, -20,   0, -20,  40,  20
	-20, -20, -20, -20, -20, -20, -20, -20,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
};

const static int king_sq_end[64] =
{
	  0,  10,  20,  30,  30,  20,  10,   0,
	 10,  20,  30,  40,  40,  30,  20,  10,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 30,  40,  50,  60,  60,  50,  40,  30,
	 30,  40,  50,  60,  60,  50,  40,  30,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 10,  20,  30,  40,  40,  30,  20,  10,
	  0,  10,  20,  30,  30,  20,  10,   0
};



Eval::Eval(Board* board)
{
  
}

int Eval::evalBoard(Board* board)
{
    int postionScore = 0;
	
	    postionScore = postionScore + calculateMaterialBalance(board) + claculatePositionValues(board);
	    if (board->turn == BLACK) return -postionScore;
		return postionScore;
	
   return 0; // should never get here.
}




// the score based purley on material blance
int Eval::calculateMaterialBalance(Board* board)
{
  int white_pcs = 0;
  int black_pcs = 0;

  for (int pcs = WR; pcs <= BP; ++pcs)
  {
  	vector<int> v_tmp = board->pList[pcs];
  	if(pcs <= WP)
  	white_pcs += v_tmp.size() * pieceVals[pcs];
    else
    black_pcs += v_tmp.size() * pieceVals[pcs];
  }

  return white_pcs - black_pcs; 
}

// the adjusmets for positional control
int Eval::claculatePositionValues(Board* board)
{
    int total_value = calcPawnsPosVal(board) + calcKnightsPosVal(board) +calcBishopPosVal(board) + calcKingPosVal(board) + calcPassedPawnsVal(board) 
    + calcIsoPawnsVal(board) + rooksAndQueensVal(board);

    return total_value;
}

int Eval::calcPawnsPosVal(Board* board)
{
	int wp_val = 0;
	int bp_val = 0;
   vector<int>v_wp = board->pList[WP];
   for(vector<int>::iterator it = v_wp.begin(), end = v_wp.end(); it != end; ++it)
   	wp_val += pawn_sq_table[sq120to64[*it]];

   vector<int>v_bp = board->pList[BP];
   for(vector<int>::iterator it = v_bp.begin(), end = v_bp.end(); it != end; ++it)
   	bp_val += pawn_sq_table[ flipBoard[sq120to64[*it]] ];

   return wp_val - bp_val;

}

int Eval::calcKnightsPosVal(Board* board)
{
	int wn_val = 0;
	int bn_val = 0;
	vector<int>v_wn = board->pList[WN];
	for (vector<int>::iterator it = v_wn.begin(), end = v_wn.end(); it != end; ++it)
		wn_val += knight_sq_table[ sq120to64[*it] ];

	vector<int>v_bn = board->pList[BN];
	for (vector<int>::iterator it = v_bn.begin(), end = v_bn.end(); it != end; ++it)
		bn_val += knight_sq_table[ flipBoard[sq120to64[*it]] ];

	return wn_val - bn_val;

}

int Eval::calcBishopPosVal(Board* board)
{
    int wb_val = 0;
	int bb_val = 0;
	vector<int>v_wb = board->pList[WB];
	for (vector<int>::iterator it = v_wb.begin(), end = v_wb.end(); it != end; ++it)
		wb_val += bishop_sq_table[ sq120to64[*it] ];

	vector<int>v_bb = board->pList[BB];
	for (vector<int>::iterator it = v_bb.begin(), end = v_bb.end(); it != end; ++it)
		bb_val += bishop_sq_table[ flipBoard[sq120to64[*it]] ];

	return wb_val - bb_val;
}

int Eval::calcKingPosVal(Board* board)
{
   int wk_val = 0;
   int bk_val = 0;
   bool gameEnding = isEndGame(board);

   int wk_pos = board->pList[WK][0]; // should be only one element in that vector.
	if (gameEnding)
		wk_val += king_sq_end[ sq120to64[wk_pos] ];
	    else
	    wk_val += king_sq_table[ sq120to64[wk_pos] ];
	

	int bk_pos = board->pList[BK][0]; // again, should only be one element in that vector.
	if (gameEnding)
		bk_val += king_sq_end[ flipBoard[ sq120to64[bk_pos]] ];
	else
		bk_val += king_sq_table[flipBoard[sq120to64[bk_pos]] ];
	
	   return wk_val - bk_val;
}
 // give bunos for passed pawns.
int Eval::calcPassedPawnsVal(Board* board)
{
	int wp_val = 0;
	int bp_val = 0;
	// find the white pawns first
	for (vector<int>::iterator it = board->pList[WP].begin(), end = board->pList[WP].end(); it != end; ++it)
	{
		uint64_t whiteMask = board->w_pawn_masks[sq120to64[*it]];
		if (!(whiteMask & board->bitboards[BP]) ) //  zero means there is no black pawn in the masked area
		{
			int passedRank = ranks[ *it ]; // get the rank, how far the passed pawn has progressed.
			wp_val += pass_pawn_val[passedRank - 1]; // the pass_pawn_val array is zero based.
		}
	}
    // now find the black pawns
    for (vector<int>::iterator it = board->pList[BP].begin(), end = board->pList[BP].end(); it != end; ++it)
    {
    	uint64_t blackMask = board->b_pawn_masks[sq120to64[*it]];
    	if (!(blackMask & board->bitboards[WP]) )
    	{
    		int passedRank = ranks [ *it ];
    		bp_val += bpass_pawn_val[ passedRank - 1 ];
    	}
    }

    return wp_val - bp_val;
}

//apply penalty for isolated pawns and double pawns
int Eval::calcIsoPawnsVal(Board* board)
{
	int wp_val = 0;
	int bp_val = 0;
 // find the white pawns
	for (vector<int>::iterator it = board->pList[WP].begin(), end = board->pList[WP].end(); it != end; ++it)
	{
		char fileName = files[*it];
        int fileNumber = fileName - 'A';
    	uint64_t white_mask = board->iso_pawn_masks[ fileNumber ];
    	if ( !(white_mask & board->bitboards[WP])) // there are no friendly pawns on adjecent files
    		wp_val += isolated_pawn_penalty;

    	// now check for double pawn.
    	int64_t wpb = board->bitboards[WP];
    	uint64_t wpsfm = board->single_file_masks[fileNumber];// the single file mask
    	clearBit(&wpb, sq120to64 [ *it ]); // remove the current pawn from the bitboard so we don't include the pawn itself in the check and always triger double pawn penalty
        if (wpsfm & wpb) // if it's non zero there is a pawn of same color on same file (double pawn)
        	wp_val += doubled_pawn_penalty; 
	}

	// now find the black pawns
    for (vector<int>::iterator it = board->pList[BP].begin(), end = board->pList[BP].end(); it != end; ++it)
    {
    	char fileName = files[*it];
    	int fileNumber = fileName - 'A';
    	uint64_t black_mask = board->iso_pawn_masks[ fileNumber ];
    	if ( !(black_mask & board->bitboards[BP])) // there are no friendly pawns on adjecent files
    		bp_val += isolated_pawn_penalty; // the penalty will end up being applied once for each pawn, so if for instance there are 3 on then same file, then 3 times

    	// check for double pawns on black side
    	int64_t bpb = board->bitboards[BP];
    	uint64_t bpsfm = board->single_file_masks[fileNumber]; // the single file mask.
    	clearBit(&bpb, sq120to64 [ *it ]); // remove the current pawn from the bitboard so we don't allways get a positive when we check for a double pawn
    	if (bpsfm & bpb) // if its non zero there is at least one other friendly pawn on the same file.
    		bp_val += doubled_pawn_penalty; // the penalty will end up being applied once for each pawn, so if for instance there are 3 on then same file, then 3 times
    }


   return wp_val - bp_val;
}

// give bunos for rooks and queens on open files, semi open files, and rook on seventh rank.
int Eval::rooksAndQueensVal(Board* board)
{
	int wrnq = 0;
	int brnq = 0;
    
    // find the white rooks
	for (vector<int>::iterator it = board->pList[WR].begin(), end = board->pList[WR].end(); it != end; ++it)
	{
	   char fileName = files[*it];
       int fileNumber = fileName - 'A';
       int rankNumber = ranks[*it];
       if ( rankNumber == 7 )
           wrnq += rook_on_seventh;

       if (isFileOpen(board, fileNumber)) // rook is on open file
       	   wrnq += rook_open_file;
       else if (isSemiOpen(board, fileNumber,WHITE))
       	    wrnq += rook_semi_open_file;
    }
    // find the black rooks
    for (vector<int>::iterator it = board->pList[BR].begin(), end = board->pList[BR].end(); it != end; ++it)
	{
       char fileName = files[*it];
       int fileNumber = fileName - 'A';
       int rankNumber = ranks[*it];
       if ( rankNumber == 2 )
       	  brnq += rook_on_seventh;

       if (isFileOpen(board, fileNumber)) // rook is on open file
       	brnq += rook_open_file;
       else if (isSemiOpen(board, fileNumber,BLACK))
       	brnq += rook_semi_open_file;
	}

	// find the white queens
	for (vector<int>::iterator it = board->pList[WQ].begin(), end = board->pList[WQ].end(); it != end; ++it)
	{
	   char fileName = files[*it];
       int fileNumber = fileName - 'A';

       if (isFileOpen(board, fileNumber)) // queen is on open file
       	 wrnq += queen_open_file;
       else if (isSemiOpen(board, fileNumber,WHITE))
       	   wrnq += queen_semi_open_file;
    }

	// and last find the black queens
	for (vector<int>::iterator it = board->pList[BQ].begin(), end = board->pList[BQ].end(); it != end; ++it)
	{
	   char fileName = files[*it];
       int fileNumber = fileName - 'A';

       if (isFileOpen(board, fileNumber)) // queen is on open file
       	   brnq += queen_open_file;
       else if (isSemiOpen(board, fileNumber,BLACK))
       	   brnq += queen_semi_open_file;
     }
      return wrnq - brnq;
}
// are there no pawn on a given file, both colors
bool Eval::isFileOpen(Board* board, int file)
{
   if ( isSemiOpen(board, file, BLACK) && isSemiOpen(board, file, WHITE) )
   	return true;
   return false;
}

// are there no pawns of same color on a given file
bool Eval::isSemiOpen(Board* board, int file, int color)
{
  uint64_t pawns = (color == WHITE) ? board->bitboards[WP] : board->bitboards[BP]; 
  if ( !(pawns & board->single_file_masks[file]) ) 
  	return true;
  return false;
}



// are we in the end game phase?, we are going to define the end game as there are two pices per side max excluding pawns and kings.
bool Eval::isEndGame(Board* board)
{
	int num_black = 0;
	int num_white = 0;
	for(int pcs = WR; pcs <= BP; ++pcs)
	{
		if ( (pcs == WP) || (pcs == WK) || (pcs == BP) || (pcs == BK) )
			continue;
		if (pcs <= WP)
		{
           num_white += board->pList[pcs].size();
		}else{
           num_black += board->pList[pcs].size();
		}
	}

  if ( (num_black <= 2) && (num_white <= 2) )
  return true;

  return false;
}

// is there a chance of a zugzwang style position?, to be used to avoid a null move forward pruning.
bool Eval::isChanceZugzwang(Board* board)
{
	int num_black = 0;
	int num_white = 0;
	for(int pcs = WR; pcs <= BP; ++pcs)
	{
		if ( (pcs == WP) || (pcs == WK) || (pcs == BP) || (pcs == BK) )
			continue;
		if (pcs <= WP)
		{
           num_white += board->pList[pcs].size();
		}else{
           num_black += board->pList[pcs].size();
		}
	}

  if ( (board->turn == WHITE) &&  (num_white <= 2) )
   	return true;
   else if ( (board->turn == BLACK) && (num_black <=2) )
   	return true;

	return false;
}


// run the eval then run it again on a mirrored board and compare the score, make sure they are identical.
void Eval::mirrorTest(Board* board)
{
    ifstream f_fen ("perft.txt");
    bool fail = false;
    string fenString;

	if ( !f_fen.is_open() )
	{
		cout << "could not open fen dbase file" << endl;
	}
    
    while (!fail)
    {
       getline(f_fen,fenString);

       if (!fenString.empty())
       {
       	 board->initBoard();
       	 board->setBoard(fenString);
       	 int before = evalBoard(board);
       	 board->mirror();
       	 int after = evalBoard(board);
         cout << "score: " << before << "    " << "mirrored: " << after << endl;

       	 if (before + after)
       	 {
       	 	cout << "error in eval!" << endl;
       	 	fail = true;
       	 }

       	 fenString.clear();
       }else
       fail = true;
   

    }
	
   
}
































