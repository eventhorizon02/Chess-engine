#include "Perft.h"

Perft::Perft(int depth, Board& board) : numberLeavs(0)
{   
	bool fail = false;
	int lineNumber = 0;
	ifstream f_perft ("perft.txt");
	if ( ! f_perft.is_open() )
	{
		cout << "could not open perft file" << endl;
	}
   
   moveMaker m_maker;
   
   cout << "starting test to 126 fen route positions..." << endl;

  while (!fail)
  {
  	board.initBoard();
    ++lineNumber;
    if(lineNumber > 125)
    {
    	fail = true;
    	cout << "finisned testing to " << lineNumber - 1 << "route positons" << endl;
    }
    cout << "begining test of route position : " << lineNumber << endl;

   string fenString;
   getline(f_perft,fenString);

   size_t pos1 = fenString.find(";D5");
   size_t pos2 = fenString.find(";D6");
   unsigned long numLen = pos2 - 1 - pos1 - 4;
   string substring = fenString.substr (pos1 + 4,numLen);
   int value = atoi(substring.c_str()); 

    board.setBoard(fenString);
    int s_time = returnMillisecs();
    devidedPerft(depth, m_maker, board);
    cout << "number of leaf nodes : " << numberLeavs << endl;
    cout << "test time : " << returnMillisecs() - s_time << endl;

    if (numberLeavs != value)
    {
    	cout << "test failed at route position " << lineNumber << endl;

    	cout << "finisned testing to " << lineNumber - 1 << "route positons" << endl;
    	fail = true;
    	
    }
 }
 
}

Perft::~Perft()
{
  
}



void Perft::perfTest(int depth, moveMaker& m_maker, Board& board )
{
   if (depth == 0)
   {
   	 ++numberLeavs;
   	 return;
   }

   MoveGen gen(&board);
   for (vector<int>::iterator it = gen.moves_v.begin(), end = gen.moves_v.end(); it != end; ++it)
   {
   	 m_maker.makeMove(*it, board);
     perfTest( depth - 1, m_maker, board);
     m_maker.takeBack(board);
   }


}

void Perft::devidedPerft(int depth, moveMaker& m_maker, Board& board)
{
   
    board.printBoard();
	cout << "\nStarting Test to Depth:" << depth << endl;
	numberLeavs = 0;

	MoveGen gen(&board);
    for (vector<int>::iterator it = gen.moves_v.begin(), end = gen.moves_v.end(); it != end; ++ it)
    {
     m_maker.makeMove(*it, board);
     long cumnodes = numberLeavs;
     perfTest( depth - 1, m_maker, board );
     m_maker.takeBack(board);
     long oldnodes = numberLeavs - cumnodes;
     cout << "move " << it - gen.moves_v.begin() << ": " << returnAlgebraic(*it) << " : " << oldnodes << endl;
    }

    cout << endl << "Test Complete : " << numberLeavs << " leaf nodes visited" << endl;

 }













