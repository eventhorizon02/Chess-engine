#ifndef __Perft__
#define __Perft__

#include "defs.h"
#include "Board.h"
#include "MoveGen.h"
#include "moveMaker.h"
#include <fstream>
#include <string>

class Perft
{
public:
	Perft(int depth, Board& board);
	~Perft();

private:
	long numberLeavs; // the number of leaf nodes in the search tree.
	void perfTest(int depth, moveMaker& m_maker, Board& board);
	void devidedPerft(int depth, moveMaker& m_maker, Board& board);
	
};



#endif
