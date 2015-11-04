             

/************************************************
 * EGM - Electronic Grand Master.               *
 * By Benny Abramovici 2015.                    *
 * Donations are acceptable through paypal at:  *
 * eventhorizon02@gmail.com                     *
 ************************************************/


#include <iostream>
#include "Board.h"
#include "IOCTRL.h"
#include "MoveGen.h"
#include "moveMaker.h"
//#include "Perft.h"
#include "Eval.h"

void performAction(int actionNumber,Board& board);

using namespace std;

int main()
{
    srand(time(NULL));
    
	Board board;
  /*
	//board.setBoard("r1b1kb1r/2pp1ppp/1np1q3/p3P3/2P5/1P6/PB1NQPPP/R3KB1R b - - 0 1");
	board.setBoard(OPENING_POSITION);
  
	IOCTRL ctrl;
    board.printBoard();

	while (1)
	{
       
      int actionResponse = ctrl.makeMove(board);
      performAction(actionResponse,board);
     
	}
	*/
	IOCTRL ctrl;
	ctrl.UCILoop();
	
   return 0;
}

void performAction(int actionNumber,Board& board)
{
	moveMaker maker;

	switch(actionNumber)
	{
		case PRINT_BOARD:
		board.printBoard();
		break;

		case TAKE_BACK:
		maker.takeBack(board);
		break;

		case MOVE_FORWARD:
		maker.moveFwd(board);
		break;

		case QUIT:
		cout << "Bye" << endl;
		exit(0);
		break;

		case MIRROR_BOARD:
		board.mirror();
		break;

		case -1:
		cout << "invalid move, please try again." << endl;
		break;

		case MOVE_MADE:
		cout << "move made" << endl;
		break;

		case NO_MOVE:
		cout << "no legal moves, game over" << endl;
		break;
	}
}















