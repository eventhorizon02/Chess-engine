EGM - Electronic Grand Master
This is a free and open source chess engine written by Benny Abramovici.
Feel free to use this engine anyway you wish, however I do request that if you are going to use this code in a program, book or toturial please give me the due credit.
I also accept donation through paypal at eventhorizon02@gmail.com

All data of the main board is held in the Board class, the board itself is an array of 120 integer, due to the way move searches are performed and the way the knight moves.
There is a convertion array that helps convert from a 64 squares based board to the 120 array.

casteling rights are stored in a one byte char, the order is KQkq, if all 4 bits are on the all casteling rights are still available. bitwise operations are used to manipulate individual bits.

The board member function setBoard is a bit long winded but all is does is take the long FEN position string parse it and sets up the board parameters accordinly.
The IOCTRL class is in charge of handling all inuput/output with command line and the GUI

a piece is considered a slider piece if it can move more than one square at a time.
The is square attacked function, returns a bool of weather a certain square is attacked by a side.

the MoveGen class is in charge of generating moves, move details are stored in 25 bits inside an int and are generated and extracted using bitwise operations.
It also has a bunch of helper functions for the use of formating and converting moves.

The MoveMaker class is in charge of actually making the move and also has the functionality required to take back a move and move back forward again.
The Pert class is for performance testing, it makes sure that the move generation of make and unmake work properly. It has no function to the engine itlesf and is used only for when developing the engine and if udates and changes are made to the engine.

The Eval class evaluates the board and gives it a score according to material balance and positions on the board. The value is increasing in positive as white has a stronger position and material blance and increases in negative for black. It also has the functions to tell if the game is checkmated or drawn.
it also checks for passed pawns and isolated pawns by using bit masks to detct a passed pawn and score it according to how far the passef pawn has advanced.

The mirror function in the Board class mirrors or flips the board and is used in testing the accuracy of the Eval functions.

The SearchMv class is responsible for searching for the best move, using alpha, beta proning algorithms.
The engine usese the UCI protocol to connect to the GUI of your choice.
You can use GUI's such as arena or jose or whichever one you prefer as long as it's a UCI supporting protocol.
The engine was compiled and tested using a mac but I'm sure that whith minor modifications to source code it'll compile on linux and windows as well.
A mac binaries is included as well.

