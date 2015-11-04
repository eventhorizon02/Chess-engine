#include "IOCTRL.h"


IOCTRL::IOCTRL()
{
  moveMaker maker;
  this->maker = maker;
}

// take a move through the command line interface
int IOCTRL:: makeMove(Board& board)
{

    MoveGen gen(&board);
    vector<int> moves_v = gen.moves_v;
	string moveString;
	cout << "enter your move or command>" << endl;
	cin >> moveString;

	if (moveString == "print")
	   return PRINT_BOARD;
	else if (moveString == "take")
	   return TAKE_BACK;
	else if (moveString == "forward")
	   return MOVE_FORWARD;
	else if (moveString == "quit")
	   return QUIT;
	else if (moveString == "mirror")
	  return MIRROR_BOARD;
	else if (moveString == "search")
     return calculate(board);
  
  int moveBits = returnMoveInt(moveString, moves_v);

   if (moveBits != -1)
   	{
   		maker.makeMove(moveBits, board);
   		return MOVE_MADE;
   	}

    return -1;
}

// return the proper move int from the list of all legal moves

 int IOCTRL::returnMoveInt(string inputString, vector<int>& moves_v)
 {
    int from, to;
    // check if input is valid 
    if (inputString.size() < 4 || inputString.size() > 5) return -1;
   else if (inputString[0] < 'a' || inputString[0] > 'h') return -1;
   else if (inputString[1] < '1' || inputString[1] > '8') return -1;
   else if (inputString[2] < 'a' || inputString[2] > 'h') return -1;
   else if (inputString[3] < '1' || inputString[3] > '8') return -1;
   else if ((inputString.size() == 5)  &&  !(inputString[4] == 'q' || inputString[4]  == 'r' || inputString[4] == 'k' || inputString[4] == 'b')) return -1;
   

    int file = inputString[0] - 'a';
    int rank = inputString[1] - '1'; // zero based board vs real game board coordinates

    from = cordToSq(file,rank);
    file = inputString[2] - 'a';
    rank = inputString[3] - '1';
    to = cordToSq(file,rank);

    // create a move int with only the to and from and promotion bits set.
    int moveMask = 0;
    moveMask |= to; // first 7 bits of the to are set.
    moveMask |= ( from << 7); // next 7 bits are set with the from square.
    
    // add the promotion to the int, if any. if no promotion was selected the first one in the vector is queen, which will be selected then.
    if (inputString.size() == 5) // we have a promotion
    {
    	char promoChar = inputString[4];
    	switch (promoChar)
    	{
           case 'q':
           (rank == RANK_8) ? moveMask |= (WQ << 18) : moveMask |= (BQ << 18);
           break;

           case 'r':
           (rank == RANK_8) ? moveMask |= (WR << 18) : moveMask |= (BR << 18);
    	   break;

           case 'k':
           (rank == RANK_8) ? moveMask |= (WN << 18) : moveMask |= (BN << 18);
    	   break;

    	   case 'b':
    	   (rank == RANK_8) ? moveMask |= (WB << 18) : moveMask |= (BB << 18);
    	   break;

        }
    }

    for (vector<int>::iterator it = moves_v.begin(), end = moves_v.end(); it != end; ++it)
    {
    	int maskingValue = (inputString.size() == 5) ? 0x3C3FFF : 0x3FFF; // using a different mask if it is a promotion versus non promotion move.
    	if (( *it & maskingValue ) == moveMask ) // we found our move in the move list.
        return *it;

    }

    return -1;
}

int IOCTRL::calculate(Board& board)
{
    s_searchParamas params;
    params.starttime = returnMillisecs();
    params.stoptime = params.starttime + 90000;
    params.depth = 1000; 
    params.stopped = false;
    params.timeset = true;
    params.nodes = 0;

    SearchMv sc;
    int bestMove = sc.getBestMove(board,&params);
    cout << "bestmove " << returnAlgebraic(bestMove) << endl;
    return bestMove;
}

// same as calculate but to be used with the GUI
void IOCTRL::searchPos(Board& board, s_searchParamas *params)
{
	params->nodes = 0;
	SearchMv sc;
	int bestMove = sc.getBestMove(board, params);
	cout << "bestmove " << returnAlgebraic(bestMove) << endl;
}

void IOCTRL::UCILoop()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
  
    char line[INPUTBUFFER];
    cout << "id name " << "EGM" << endl;
    cout << "id author Benny Abramovici" << endl;
    cout << "uciok" << endl;
    
     Board board; 
     s_searchParamas params;
    
     while (1) 
     {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if (!fgets(line, INPUTBUFFER, stdin))
        continue;

        if (line[0] == '\n')
        continue;

        if (!strncmp(line, "isready", 7))
         {
            cout << "readyok" << endl;
            continue;
        } else if (!strncmp(line, "position", 8)) {
            ParsePosition(line, &board);
        } else if (!strncmp(line, "ucinewgame", 10)) 
        { 
            string str = "position startpos\n";
            char * writable = new char[str.size() + 1];
            copy(str.begin(), str.end(), writable);
            writable[str.size()] = '\0'; 
            ParsePosition(writable, &board);
            delete[] writable;
        } else if (!strncmp(line, "go", 2)) {
            ParseGo(line, &params, &board);
        } else if (!strncmp(line, "quit", 4)) {
            params.quit = true;
            break;
        } else if (!strncmp(line, "uci", 3))
         {
            cout << "id name " << "EGM" << endl;
            cout << "id author Benny Abramovici" << endl;
            cout << "uciok" << endl;
         }
     if(params.quit) break;
    }
  }
  
// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void IOCTRL::ParseGo(char* line, s_searchParamas *params, Board* board)
{
     params->nodes = 0;
     params->starttime = 0;
     params->stoptime = 0;
     params->timeset = false;
     params->stopped = false;
     params->quit = false;
     params->movestogo = 30;

	int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
    char *ptr = NULL;
	
	
	if ((ptr = strstr(line,"infinite"))) {
		;
	} 
	
	if ((ptr = strstr(line,"binc")) && board->turn == BLACK) {
		inc = atoi(ptr + 5);
	}
	
	if ((ptr = strstr(line,"winc")) && board->turn == WHITE) {
		inc = atoi(ptr + 5);
	} 
	
	if ((ptr = strstr(line,"wtime")) && board->turn == WHITE) {
		time = atoi(ptr + 6);
	} 
	  
	if ((ptr = strstr(line,"btime")) && board->turn == BLACK) {
		time = atoi(ptr + 6);
	} 
	  
	if ((ptr = strstr(line,"movestogo"))) {
		movestogo = atoi(ptr + 10);
	} 
	  
	if ((ptr = strstr(line,"movetime"))) {
		movetime = atoi(ptr + 9);
	}
	  
	if ((ptr = strstr(line,"depth"))) {
		depth = atoi(ptr + 6);
	} 
	
	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}
	
	params->starttime = returnMillisecs();
	params->depth = depth;
	
	if(time != -1) {
		params->timeset = true;
		time /= movestogo;
		time -= 50;		
		params->stoptime = params->starttime + time + inc;
	} 
	
	if(depth == -1) {
		params->depth = 64;
	}
	
	cout << "time:" << time << " start:" << params->starttime << " stop:" << params->stoptime << " depth:" << params->depth << " timeset:" << params->timeset << endl;
	searchPos(*board, params);

}

// position fen fenstr
// position startpos
// ... moves e2e4 e7e5 b7b8q
// position fen r5rk/1pp1qpn1/p2p3p/3P4/2PQNP1b/1P3B1P/P6K/5RR1 w - - 0 31 moves e2e4 e7e5 b7b8q
// parse the position message from the GUI
void IOCTRL::ParsePosition(char* lineIn, Board* board)
{
	string lineStr(lineIn); // converts char* to std::string.
    size_t found = lineStr.find("startpos");
	if (found != string::npos)
		board->setBoard(OPENING_POSITION);
	else // it's rather an fen 
	{
       found = lineStr.find("fen");
       found += 4; // move to the first character of the fen string.
       size_t endFen = lineStr.find("moves");
       if (endFen != string::npos)
       	endFen --; // the real end pos of the fen string.
       else endFen = lineStr.size(); // the where no moves added after the fen string so the end of the fen info is at the end of lineStr.
       string fenString = lineStr.substr (found, endFen - found);
       board->setBoard(fenString);
	}
      found = lineStr.find("moves");
      if (found != string::npos) // we found an additional move list
      {
      	int move;
      	found += 6;
      	while (found <= lineStr.size())
      	{
      		size_t endMv = lineStr.find(" ",found,1);
      		if (endMv == string::npos)
      		    endMv = lineStr.size() - 1;
      			string newMv = lineStr.substr (found, endMv - found);
      			if (newMv.size() < 4) break;
      			MoveGen gen(board);
                vector<int> moves_v = gen.moves_v;
      		    move = returnMoveInt(newMv,moves_v);
      		    if(move == -1) break;
			    maker.makeMove(move, *board);
			    found = endMv + 1;
      	}
      }
     board->printBoard();
}










