#include "moveMaker.h"

#define MV_STRCT board.board_hist
#define MV_STACK board.hist_stack

moveMaker::moveMaker()
{

}

moveMaker::~moveMaker()
{

}

void moveMaker::makeMove(const int move, Board& board)
{
	// before making any changes store board details in the history vector
	storeHist(move, board);

	// find out if it was a pawn move or a capture move so we can reset to zero the fifty move rule counter.
	int piece = board._board[ GET_FROM(move) ]; // what piece has been moved
	if ( (piece == WP) || (piece == BP) || ( GET_CAPT_PCS(move) != EMPTY) )
		board.hm_clock = 0;
		else
		++board.hm_clock; // if it wasn't a pawn move or a capture increment the half move clock.
      
       setBitboards(move, board); // set up not only bitboards but pList, casteling rights, en-pas, etc.
       if (board.turn == BLACK)
       ++board.fm_number; // if we just performed blacks move then time to increment the full move clock by one.
       board.turn ^= 1; // switch sides
       board.remakeBoard();
       board.hashKey = board.genHashKey(); // generates a unique to the position 64 bit hash key.

     //printMoveDetails(move);

}
void moveMaker::takeBack(Board& board)
{
	if (MV_STRCT.empty())
	cout << "no more moves to take back " << endl;
	else{
	s_hist temp = MV_STRCT.back();
    remakeBitboards( temp, board );
	board.remakeBoard();
    MV_STACK.push_back(temp);
    MV_STRCT.pop_back();

    } 

 }

// make a null move, used in null move forward pruning.
 void moveMaker::makeNullMove(Board& board)
 {
 	// there is always going to be a king on the board, so we'll find where it is and move it to the same spot(no move basically).
 	int fromTo = board.pList[WK][0];
 	assert(board.pList[WK].size() == 1);
 	int move = formatNullMove(fromTo,fromTo,EMPTY,EMPTY,false,false,false,false);
 	makeMove(move,board);
 	
 }

// take back a null move. 
void moveMaker::takeNullMove(Board& board)
{
   takeBack(board);
 }


void moveMaker::moveFwd(Board& board)
{
	if ( MV_STACK.empty() )
	cout << "no more moves to go fwd  " << endl;
   else
   {
   	  s_hist s_next = MV_STACK.back();
      int bits = s_next.bits;
      
     // find out if it was a pawn move or a capture move so we can reset to zero the fifty move rule counter.
	int piece = board._board[ GET_FROM(bits) ]; // what piece has been moved
	if ( (piece == WP) || (piece == BP) || ( GET_CAPT_PCS(bits) != EMPTY) )
		board.hm_clock = 0;
		else
		++board.hm_clock; // if it wasn't a pawn move or a capture increment the half move clock.

	if (board.turn == BLACK)
       ++board.fm_number; // if we just performed blacks move then time to increment the full move clock by one.
       board.turn ^= 1; // switch sides


      setBitboards(bits, board);
      board.remakeBoard();
      board.hashKey = board.genHashKey(); // generates a unique to the position 64 bit hash key.
      MV_STRCT.push_back(s_next);
      MV_STACK.pop_back();
    }
}

// for testing purposes only. remove before flight
void moveMaker::printMoveDetails(int move)
{
	cout << "move details:" << endl;
	cout << "move number:" << move << endl;
	cout << returnAlgebraic(move) << endl;
	cout << "captured piece was: " << piece_symbols[GET_CAPT_PCS(move)] << endl;
	cout << "pawn promoted to  : " << piece_symbols[GET_PROMO_PCS(move)] << endl;
	cout << "was it a promotion: " << IS_PROMO(move) << endl;
	cout << "was it enpass capt: " << IS_EN_PASS_CAPT(move) << endl;
	cout << "was it pawn start : " << IS_PAWN_START(move) << endl;
	cout << "was it castel move: " << IS_CASTLE(move) << endl;

}

// store the details in the hist vector

void moveMaker::storeHist(const int move, Board& board)
{
	s_hist mvInfo;
	mvInfo.bits = move;
	mvInfo.side = board.turn;
	mvInfo.enPasSq = board.enPasSq;
	mvInfo.hm_clock = board.hm_clock;
	mvInfo.cast_rghts = board._cast_rgts;
	mvInfo.hashKey = board.hashKey;

	MV_STRCT.push_back(mvInfo);
    MV_STACK.clear(); // after making a move, it should be always considered the last move so we clear the history stack.
	
 }

// set the bitboards and the Plist and casteling rights and enpas up from the move int

void moveMaker::setBitboards(const int move, Board& board)
{
  int from = GET_FROM(move);
  int movingPiece = board._board[ from ];
  int to = GET_TO(move);
  int captPiece = GET_CAPT_PCS(move);
  int promoPiece = GET_PROMO_PCS(move);
  bool pawnStart = IS_PAWN_START(move);
  bool isCastle = IS_CASTLE(move);
  bool isEnPasCapt = IS_EN_PASS_CAPT(move);

  // cancell en-pass square
  board.enPasSq = NO_SQ;

  // perform the casteling move.

  if (isCastle)
  {
  	if (movingPiece == WK)
  	{
  		movePiece(WK, from, to, board);
  		if (to == G1)
  			movePiece(WR, H1, F1, board ); // castle king side
  		else
  			movePiece(WR, A1, D1, board ); // castle queen side

  		// now cancell white king casteling rights
  		 board.setCastRgt (WQC, false); 
  		 board.setCastRgt (WKC, false); 

  	}else{
  		movePiece(BK, from, to, board);
  		if (to == G8)
  			movePiece(BR, H8, F8, board );
  		else
  			movePiece(BR, A8, D8, board );

  		board.setCastRgt ( BQC, false );
  		board.setCastRgt ( BKC, false );

  	}
  }else if (promoPiece != EMPTY) // it's a promotion move
  {
  	 if (captPiece != EMPTY) // it's a capturing type promotion move.
     {
     	removePiece( captPiece, to, board);
     	removePiece( movingPiece, from, board );
     	assert(movingPiece == WP || movingPiece == BP);
     	assert( (ranks[to] == 8) || (ranks[to] == 1) );
        assert( (ranks[from] == 7) || (ranks[from] == 2) );
     	setPiece ( promoPiece, to, board );
     	// in case the capture was a rook we have to cancell the proper castelling rights.
     	if ( to == H8 )
     		board.setCastRgt ( BKC, false );
     	else if ( to == A8 )
     		board.setCastRgt ( BQC, false );
     	else if ( to == H1 )
     		board.setCastRgt ( WKC, false );
     	else if ( to == A1 )
     		board.setCastRgt ( WQC, false );
     	// even if there was no rook at these corner squares the we have just re- set the no casteling rights on these sides.

     }else{// it's a straight in promotion move, with no capture.
     	removePiece (movingPiece, from, board);
     	assert(movingPiece == WP || movingPiece == BP);
     	assert( (ranks[to] == 8) || (ranks[to] == 1) );
        assert( (ranks[from] == 7) || (ranks[from] == 2) );
     	setPiece ( promoPiece, to, board );
     }
  }else if (pawnStart) // it's a pawn start
  {
    movePiece( movingPiece, from, to, board);
  	if ( ranks[from] == 2 ) // white pawn start
  		board.enPasSq = cordToSq( (int)(files[from] - 'A'), RANK_3 );
  	else // Black pawn start
  		board.enPasSq = cordToSq( (int)(files[from] - 'A'), RANK_6 );

  }else if (isEnPasCapt) // it's an en-passant style capture.
  {
  	movePiece(movingPiece, from, to, board);
  	assert( (captPiece == BP) || (captPiece == WP) );
  	if (captPiece == BP)
  	{
       int victimPawnPos = cordToSq( (int)(files[to] - 'A'), RANK_5);
       removePiece(BP, victimPawnPos, board);
  	}else
  	{
  		assert(captPiece == WP);
  		int victimPawnPos = cordToSq( (int)(files[to] - 'A'), RANK_4);
  		removePiece(WP, victimPawnPos, board);
  	}

  }else if ( captPiece != EMPTY ) // it's a capturing move.
  {
  	removePiece( captPiece, to, board );
  	movePiece ( movingPiece, from, to, board);
  	// check if any casteling right were lost.
  	if ( to == H8 )
  		board.setCastRgt( BKC, false );
  	else if ( to == A8 )
  		board.setCastRgt( BQC, false );
  	else if ( to == H1 )
  		board.setCastRgt (WKC, false );
  	else if ( to == A1 )
  		board.setCastRgt (WQC, false );

  	if ( from == E1 ) // White king was moved
  	{
  		board.setCastRgt (WQC, false);
  		board.setCastRgt (WKC, false);
  	}
  	else if ( from == H1 )
  		board.setCastRgt ( WKC, false);
  	else if ( from == A1 )
  		board.setCastRgt ( WQC, false);
  	else if ( from == E8 )// Black king was moved
  	{
  		board.setCastRgt (BQC, false);
  		board.setCastRgt (BKC, false);
    }
    else if ( from == H8 )
    	board.setCastRgt( BKC, false);
    else if ( from == A8 )
    	board.setCastRgt( BQC, false);

  }else // it's a quite move
  {
  	movePiece ( movingPiece, from, to, board);

  	if ( from == E1 ) // White king was moved
  	{
  		board.setCastRgt (WQC, false);
  		board.setCastRgt (WKC, false);
  	}
  	else if ( from == H1 )
  		board.setCastRgt ( WKC, false);
  	else if ( from == A1 )
  		board.setCastRgt ( WQC, false);
  	else if ( from == E8 )// Black king was moved
  	{
  		board.setCastRgt (BQC, false);
  		board.setCastRgt (BKC, false);
    }
    else if ( from == H8 )
    	board.setCastRgt( BKC, false);
    else if ( from == A8 )
    	board.setCastRgt( BQC, false);
}


}

// set up bitboards and other data after a take back command or move forward command.

void moveMaker::remakeBitboards( const s_hist& mv_struct, Board& board )
{
   int bits = mv_struct.bits; // get the move int out of the struct
   board.turn = mv_struct.side; // reset the turn.
   board.enPasSq = mv_struct.enPasSq; // reset the en-passent square
   board.hm_clock = mv_struct.hm_clock; // reset the half move clock
   board._cast_rgts = mv_struct.cast_rghts; // restore the casteling rights info.
   board.hashKey = mv_struct.hashKey; // reset the hash key rather than caluculate it again.
   if (board.turn == BLACK)
   	--board.fm_number; // if its Black's turn than it means we took back a blacks move so we decrement the full move clock by one.

  int from = GET_FROM(bits);
  int to = GET_TO(bits);
  int movingPiece = board._board[ to ]; // it will be the moving piece unless if we take back after a promotion.
  int captPiece = GET_CAPT_PCS(bits);
  int promoPiece = GET_PROMO_PCS(bits);
  bool isCastle = IS_CASTLE(bits);
  bool wasEnPasCapt = IS_EN_PASS_CAPT(bits);

  if (isCastle) // it was a casteling move.
  {
  	if(movingPiece == WK) // white casteled
  	{
  		if ( to == G1 ) // white king castle
  		{
  			movePiece( WK, G1, E1, board );
  			assert(board._board[F1] == WR);
  			movePiece(WR, F1, H1, board );

  		}else
  		{
  			assert ( to == C1 ); // white queen castle
  			movePiece ( WK, C1, E1, board );
  			assert(board._board[D1] == WR );
  			movePiece (WR, D1, A1, board );
  		}
  	}else{
  		assert(movingPiece == BK); // Black casteled
  		if ( to == G8 ) // black king castle
  		{
  			movePiece( BK, G8, E8, board );
  			assert(board._board[F8] == BR);
  			movePiece( BR, F8, H8, board );
  		}else{
  			assert ( to == C8 ); // Black queen castle
  			movePiece ( BK, C8, E8, board);
  			assert (board._board[D8] == BR );
  			movePiece (BR , D8, A8, board);
  		}
  	}
  }else if ( promoPiece != EMPTY ) // it was a promotion move
  {
  	 if ( captPiece != EMPTY ) // it was a capture style promotion.
  	 {
  	 	removePiece(promoPiece, to, board);
  	 	setPiece( captPiece, to, board );
        // now we have to recover the moving piece which is different in the promotion case
        int altMovingPiece = (ranks[to] == 8 ) ? WP : BP;
        setPiece( altMovingPiece, from, board );
  	 	assert( (altMovingPiece == WP) || (altMovingPiece == BP) );
  	 }else{ // it was a straight in promotion
  	 	removePiece( promoPiece, to, board );
  	 	int altMovingPiece = (ranks[to] == 8 ) ? WP : BP;
  	 	setPiece (altMovingPiece, from, board);
  	 	assert( (altMovingPiece == WP) || (altMovingPiece == BP) );
     }
  }else if ( wasEnPasCapt ) // it was an en-passant capture
  {
  	movePiece(movingPiece, to, from, board);
  	assert ( (captPiece == WP) || (captPiece == BP) );
  	if (captPiece == BP)
  	{
  		// add a black pawn at (to file, 5)
      int pawnPos = cordToSq( (int)(files[to] - 'A'), RANK_5 );
      setPiece(BP, pawnPos, board);

  	}else
  	{
  		assert(captPiece == WP);
  		// add a white pawn at (to file, 4)
  		int pawnPos = cordToSq( (int)(files[to] - 'A'), RANK_4 );
  		setPiece(WP, pawnPos, board);
  	}


  }else if ( captPiece != EMPTY ) // it was a capture move
  {
  	removePiece(movingPiece, to, board);
  	setPiece (captPiece, to, board );
  	setPiece (movingPiece, from, board);
  }else // it was a quite move
  movePiece(movingPiece, to, from, board);
}

// helper functions.

void moveMaker::movePiece(const int piece, const int from, const int to, Board& board)
{
	removePiece(piece, from, board);
    setPiece(piece, to, board);
}

// remove a piece from a given position

void moveMaker::removePiece(const int piece, const int position, Board& board) 
{
	// first remove the piece from the bitboard.
    clearBit(&board.bitboards[piece], sq120to64[ position ] );

    // then remove the piece from the pList
    for (vector<int>::iterator it = board.pList[ piece ].begin(), end = board.pList[ piece ].end(); it != end; ++it)
    {
    	if (*it == position)
    	{
    		board.pList[ piece ].erase(it);
    	    break;
    	}
    }
	
}

// set a piece at a given position.
void moveMaker::setPiece(const int piece, const int position, Board& board)
{
	  setBit(&board.bitboards[piece], sq120to64[ position ] );
    board.pList [ piece ].push_back( position );

}
// create an int dummy move
int moveMaker::formatNullMove(int from, int to, int cptPc, int promoPc ,bool promote, bool epCapt, bool pawnStrt, bool cstl)
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


















