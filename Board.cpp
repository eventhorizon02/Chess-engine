#include "Board.h"

Board::Board()
{
	initBoard();
}

void Board::initBoard() // sets internal 64 squares to empty and border squares to border on the 120 squares array.
{
	for (int i = 0; i < BRD_SQS_NUM; ++i )
	_board[i] = OFFBOARD;
   
    for (int i = 0; i < 64; ++i)
    	_board[ sq64to120[i] ] = EMPTY;
    
     memset(bitboards,0,sizeof(bitboards));
     memset(_color,NONE,sizeof(_color));

    for (int i = 0; i < 12; ++i)
    pList[i].clear();
    
    turn = NONE;
    _cast_rgts = 15; // equivalent to binary 1111 means all casteling rights are on. the positions are KQkq.
    enPasSq = NO_SQ; // no en passant square before first move.
    fm_number = 0; // it starts at one when board is in the opening position.
    hm_clock = 0;
    hashKey = 0;
    board_hist.clear(); 
    hist_stack.clear();
    initHashArrays();
    initMaskArrays(); // init the arrays holsding the bit masks used to detect passed pawns in the Eval class.
    initMvvLvaArrays(); // init the array that holds the ordering scores of the moves in the mvv/lva fashion.
}

//initiallises the hash arrays with random numbers, to be used in generating the unique hash position key;
void Board::initHashArrays()
{
	for (int piece = WR; piece <= BP; ++piece)
	{
		for (int square = 0; square < 64; ++ square)
		{
			pieces_hashkey[piece][square] = genRand64();
		}
	}
        for (int i = 0; i < 64; ++i)
		enPassant_hash[i] = genRand64();
        
        for (int i = 0; i < 16; ++i)
        	castlePerm_hash[i] = genRand64();

        side_hashKey = genRand64();
    }

// generates a unique to the position 64 bit hash key.
uint64_t Board::genHashKey()
{
	uint64_t hash_key = 0LL;
	for (int i = 0; i < 64; ++i)
	{
		int tmpPiece = _board[ sq64to120[i] ];
		if ( tmpPiece != EMPTY )
			hash_key ^= pieces_hashkey[tmpPiece][i];
	}

	if (side_hashKey == WHITE)
		hash_key ^= side_hashKey;
	if (enPasSq != NO_SQ)
		hash_key ^= enPassant_hash[enPasSq];
	hash_key ^= castlePerm_hash[_cast_rgts];

    return hash_key;
}
// initialses the bit masks array used in detecting a passed pawn and isolated pawn in the Eval class
void Board::initMaskArrays()
{
   for (int FILE = FILE_A; FILE <= FILE_H; ++FILE)
   {
   	  for (int RANK = RANK_1; RANK <= RANK_8; ++RANK)
   	  {
   	  	int64_t wtemp = 0LL;
   	  	int64_t btemp = 0LL;

   	  	int subjectSq = sq120to64[ cordToSq(FILE,RANK) ];
   	  	w_pawn_masks[subjectSq] = 0LL;
   	  	b_pawn_masks[subjectSq] = 0LL;
   	  	int fr_file = max( 0, FILE - 1 );
   	  	int to_file = min( 7, FILE + 1 );

        
   	  	for (int files = fr_file; files <= to_file; ++files)
   	  	{
   	  		// set up the white pawn masks
   	  		int w_strt_rank = RANK + 1;
   	  		while(w_strt_rank <= RANK_8)
   	  		{
   	  			setBit( &wtemp, sq120to64 [ cordToSq(files, w_strt_rank)] );
   	  			++w_strt_rank;
   	  		}

   	  		// set up the black pawns masks
   	  		int b_strt_rank = RANK - 1;
   	  		while (b_strt_rank >= RANK_1)
   	  		{
   	  			setBit ( &btemp, sq120to64 [ cordToSq(files, b_strt_rank)] );
   	  			--b_strt_rank;

   	  		}
   	  	}
          
   	  	  w_pawn_masks[subjectSq] = wtemp;
   	  	  b_pawn_masks[subjectSq] = btemp;  
   	  }
      // set up the isolated pawn masks and the single file masks
      int64_t mask_temp = 0LL;
      int64_t mask_temp_single = 0LL;
      int left_file = ( FILE > FILE_A) ? FILE - 1 : FILE + 1;
      int right_file = ( FILE < FILE_H) ? FILE + 1 : FILE - 1; // we want to mark the files to the left and right of the current file
      for (int maskRanks = RANK_1; maskRanks <= RANK_8; ++ maskRanks)
      {
          setBit ( &mask_temp, sq120to64 [ cordToSq (left_file, maskRanks) ] );
          setBit ( &mask_temp, sq120to64 [ cordToSq (right_file, maskRanks) ] );
          setBit ( &mask_temp_single, sq120to64 [ cordToSq (FILE, maskRanks)] );
      }

      iso_pawn_masks[ FILE ] = mask_temp;
      single_file_masks[ FILE ] = mask_temp_single;
   }

}

// init the array that holds the ordering scores of the moves in the mvv/lva fashion.
void Board::initMvvLvaArrays()
{
   for (int attacker = WR; attacker <= BP; ++attacker)
   {
   	  for (int victim = WR; victim <= BP; ++victim)
   	  mvvlva[victim][attacker] = victimScores[victim] + 6 - (victimScores[attacker] / 100);
   }
}


void Board::printBoard()
{
	cout << endl;
	
  for (int RANK = RANK_8; RANK >= RANK_1; --RANK)
  {
  	 cout  << RANK + 1 << "  ";
  	for (int FILE = FILE_A; FILE <= FILE_H; ++FILE)
  	{
  		int targetSq = (RANK * 8) + FILE;
  		int val = _board[ sq64to120 [ targetSq ]];
  		char symbol = piece_symbols[val];
  		cout << " " << symbol << " ";
  	} 
  	cout << endl << endl;
  }

  cout << "   "; 
  for (int FILE = FILE_A; FILE <= FILE_H; ++FILE)
  cout << " " << (char)(65 + FILE) << " "; // prints out letters A TO H at the bottom of thr board 65 is the ascii code for A
  

  cout << endl << endl;
  if (turn == WHITE)
  cout << "White's move" << endl;
  else if (turn == BLACK)
  cout << "Black's move" << endl;
  else
  	cout << "Error" << endl;

  cout << "WKC:" << extractCastRgt(WKC) << "  WQC:" << extractCastRgt(WQC) << "  BKC:" << extractCastRgt(BKC) << "  BQC:" << extractCastRgt(BQC) << endl;
  cout << "En Passant:";
  if (enPasSq != NO_SQ)
  {
  	cout << (char)(getFile(enPasSq) + 'A') << getRank(enPasSq) + 1;
  }
  else
  	cout << "-";
    cout << "    hm clock:" << hm_clock << "    fm counter:" << fm_number << endl;
    cout << "hask key: " << hashKey << endl;
}

void Board::printColors()
{
	for (int i = 0; i < 64; ++i)
	{
		cout << (int) _color[i] << " ";
		if ((i + 1) % 8 == 0)
			cout << endl;
	}
}

// print the content of a bitboard, for testing purposes only
void Board::printBitboard(int64_t bitboard)
{
    for (int i = 0; i < 64; ++i)
    {
    	(retreiveBit (&bitboard, i)) ? cout << "x" : cout << "-";
    	if ((i + 1) % 8 == 0)
			cout << endl;
    }
}

// this method sets the board up from the pList array/bitboards info or from the bitboards

void Board::setBoard(const string& FEN_String)
{
	initBoard();
	parseFEN(FEN_String);
	for (int i = 0; i < 64; ++i)
	_board[sq64to120[i]] = EMPTY;

     setBitboardsAndColors();
    /*
	for (int i = 0; i < 12; ++i)
	{
	   vector<int> v = pList[i];
      for (vector<int>::iterator it = v.begin(),end = v.end(); it != end; ++it )
      	_board[*it] = i;
	}
	*/
	// alternative board set up form bitboards
	for (int i = 0; i < 12; ++i)
	{
		int64_t b_board = bitboards[i];
        for (int j = 0; j < 64; ++j)
        {
        	if (retreiveBit(&b_board, j))
			_board[sq64to120[j]] = i;
        }
    }
    
    hashKey = genHashKey(); // generate a unique hash key to the position.
 }

 void Board::remakeBoard() // set up board again from bitboards after a move was made
 {
 	for (int i = 0; i < 64; ++i)
	_board[sq64to120[i]] = EMPTY;
   
    for (int i = 0; i < 12; ++i)
	{
		int64_t b_board = bitboards[i];
        for (int j = 0; j < 64; ++j)
        {
        	if (retreiveBit(&b_board, j))
			_board[sq64to120[j]] = i;
        }
    }
    
    // set up the color char array
    memset(_color,NONE,sizeof(_color));
   for (int i = 0; i < 12; ++i)
   {
   	 vector<int> piecesVector = pList[i];
   	 for (vector<int>::iterator it = piecesVector.begin(),end = piecesVector.end(); it != end; ++it)
   	      _color[sq120to64[*it]] = ((i >= BR) && (i < EMPTY)) ? BLACK : WHITE;
   }

 }

// this method parses a FEN string and sets pList and bitboards

void Board::parseFEN(const string& FEN_String)
{
   int RANK = RANK_8;
   int FILE = FILE_A;
   int index = 0;
   char ch;
   bool found = false;
   while (RANK >= RANK_1)
   {
     ch = FEN_String[index];
     if ( (ch == '/') || (FILE > FILE_H) ) // end of rank info symbol or end of field
     {
     	--RANK;
     	FILE = FILE_A;
     	++index;
     } else // it's either a piece symbol or a number representing the number of empty spaces
     {
        // check if we have a piece symbol match
        for (int i = 0; i < 14; ++i) 
        {
        	if (ch == piece_symbols[i])
        	{
        		pList [i].push_back(sq64to120[ 8 * RANK + FILE ]);
        		++FILE;
     	        ++index;
     	        found = true;
     	        break;
     	    }
     	}
       if (!found) // it must be a number then.
        {
           int num = ch - 48; // convert ascii char to int.
           for (int i = num; i >= 1; --i)
            ++FILE;
           
           ++index;
        }else
        found = false;
     }
   }
   // now read who's turn it is.
   ch = FEN_String[index];
   if (ch == 'w')
   	turn = WHITE;
   else if (ch == 'b')
   	turn = BLACK;
   else
   	turn = NONE;

   // process the casteling rights from the string
   index +=2;
   _cast_rgts = 0; // initially set no casteling rights
   for (int i = 0; i <= 4; ++i) // go all the way to  a space. any number bigger than 4 will do.
   {
   	 ch = FEN_String[index];
     if  (ch == ' ')  //  we've reached the end of casteling info
      break;
    else if (ch == '-')
    {
      ++index;
      break;
    }

     switch (ch)
   	 {
   	 	case 'K':
   	 	setCastRgt(WKC,true);
   	 	break;

   	 	case 'Q':
   	 	setCastRgt(WQC,true);
   	 	break;

   	 	case 'k':
   	 	setCastRgt(BKC,true);
   	 	break;

   	 	case 'q':
   	 	setCastRgt(BQC,true);
   	 	break;
    }
     ++index;
   	}
    assert (FEN_String[index] == ' ');
    
   // read the en-passant square info.
   enPasSq = NO_SQ;
   if (FEN_String[index + 1] == '-')
    index += 3;
   else
   {
   char file = FEN_String[++index] - 'a';
   char rank = FEN_String[++index] - '1';
   enPasSq = cordToSq(file,rank);
   index +=2;
   }
   // read the half move clock and full move number info.
   hm_clock = FEN_String[index] - '0';
   fm_number = FEN_String[index + 2] - '0';

 }

// sets up bitboards from pList array and sets the colors array as well

void Board::setBitboardsAndColors()
{
	memset(_color,NONE,sizeof(_color));
   for (int i = 0; i < 12; ++i)
   {
   	 vector<int> piecesVector = pList[i];
   	 for (vector<int>::iterator it = piecesVector.begin(),end = piecesVector.end(); it != end; ++it)
   	 {
   	 	int index = sq120to64[*it];
   	 	setBit(&bitboards[i], index);
   	 	_color[index] = ((i >= BR) && (i < EMPTY)) ? BLACK : WHITE;
   	 }
   }
}



// returns a bool for a casteling right in question.
bool Board::extractCastRgt (int place) const
{
	char bits = _cast_rgts;
	return bits & (1 << place);
}

// sets a particular casteling right to on or off.
void Board::setCastRgt (int place, bool cstRght) 
{
    _cast_rgts |= (1 << place);
    if (!cstRght)
    	_cast_rgts ^= (1 << place);
}

int Board::getRank(int sq_num) // returns the rank number for a given square number on the 120 sq board.
{
  int firstDigit = sq_num / 10;
  if (firstDigit < 2 || firstDigit > 9)
    return OFFBOARD;
    return firstDigit - 2;
}
int Board::getFile(int sq_num) // returns the file number for a given square number on the 120 sq board.
{
  int secondDigit = sq_num % 10;
  if (secondDigit < 1 || secondDigit > 8)
    return OFFBOARD;
    return secondDigit - 1;
}

// return a bool weather a certian square is attacked by side.

 bool Board::isSqAttacked(int sq, int side) const
 {
 	int tmp_piece;
 	int dirIndx;
   if (side == WHITE)
 	{
      if ( (_board[sq - 9] == WP) || (_board[sq - 11] == WP) ) // pawn attack
 			return true;
    }else
 	{
 		if ( (_board [sq + 9] == BP) || (_board [sq + 11] == BP)) // pawn attack
 			return true;
 	}
 
 	// check for knights or king attacks
 	for (int index = 0; index < 8; index++)
 	{
 		tmp_piece = _board[sq + pieceVectors[KNIGHT][index]];
 		if ( ( (tmp_piece == WN) && (side == WHITE) ) ||  ( ( tmp_piece == BN ) && (side == BLACK) ) ) // knight attack
 			return true;

        tmp_piece = _board[sq + pieceVectors[KING][index]];
        if ( ( (tmp_piece == WK) && (side == WHITE) ) ||  ( ( tmp_piece == BK ) && (side == BLACK) ) ) // king attack
 			return true;
    }
    
     // now check for the silder type pieces (rook, queen, bishop)
    for (int directions = 0; directions < 8; ++directions)
  {
    dirIndx = pieceVectors[QUEEN][directions]; // this one has all 8 directions so we can use the queen directions vectors
    int testSq = sq + dirIndx;
    while (_board[testSq] != OFFBOARD)
    {
    	tmp_piece = _board[testSq];
    	if ( tmp_piece != EMPTY ) 
    	{
    	  
           if ( ((pieceType[tmp_piece] == ROOK) || ( pieceType[tmp_piece] == QUEEN )) && ( directions < 4) && (_color[ sq120to64[testSq] ] == side) ) // rook and queen movements in the first 4 indices
           return true;
           
            if ( ((pieceType[tmp_piece] == BISHOP) || ( pieceType[tmp_piece] == QUEEN )) && ( directions >= 4) && (_color[ sq120to64[testSq] ] == side) ) // bishop and queen movemnts in the last 4 indices
           return true;

          break;  // break of the while loop since we found a piece.
       }

       testSq += dirIndx;
    }
  }

   return false;
 }

 bool Board::isInCheck (int side) const // is side in check
 {
    // find the square where the king in question sits
    vector<int> kingVector = (side == WHITE) ? pList[WK] : pList[BK];
    assert(kingVector.size() == 1);
    int kingSq = kingVector[0]; // should be only one piece in this vector and thats the king.
    return isSqAttacked(kingSq, side^1);
 }

 // mirrors the board and is used in testing the evaluation functions.
void Board::mirror()
{
	vector <int> m_pList [12]; // holds the square numbers of each piece on the board.
	int64_t m_bitboards[12];

	for (int i = 0; i < 12; ++i)
    m_pList[i].clear();
    memset(m_bitboards,0,sizeof(bitboards));

	for (int pcsTyp = WR; pcsTyp <= BP; ++ pcsTyp)
	{
		for(vector<int>::iterator it = pList[pcsTyp].begin(), end = pList[pcsTyp].end(); it != end; ++it)
		{
			m_pList [ flipPiece[pcsTyp] ].push_back(sq64to120[ flipBoard[ sq120to64 [ *it ] ] ]);
			setBit(&m_bitboards [ flipPiece[pcsTyp]],flipBoard[ sq120to64 [ *it ] ]);    
		}
	}

	for (int i = 0; i < 12; ++i)
	{
		pList[i] = m_pList[i];
		bitboards[i] = m_bitboards[i];
	}
     remakeBoard();
     turn ^= 1;
     if (enPasSq != NO_SQ)
     	enPasSq = sq64to120 [ flipBoard[ sq120to64 [ enPasSq ] ] ];

     char m_cst_rgts = 0;
    
     if (extractCastRgt (WKC))
     	m_cst_rgts |= (1 << BKC);

     if (extractCastRgt (WQC))
       m_cst_rgts |= (1 << BQC);

     if (extractCastRgt (BKC))
        m_cst_rgts |= (1 << WKC);

     if (extractCastRgt (BQC))
     	m_cst_rgts |= (1 << WQC);

      _cast_rgts = m_cst_rgts;

      hashKey = genHashKey();
     
}













