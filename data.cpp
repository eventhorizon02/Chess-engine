#include <cstdint>
enum { ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN }; 

static const int sq64to120[64] = { 21, 22, 23, 24, 25, 26, 27, 28,
                                   31, 32, 33, 34, 35, 36, 37, 38,
                                   41, 42, 43, 44, 45, 46, 47, 48,
                                   51, 52, 53, 54, 55, 56, 57, 58,
                                   61, 62, 63, 64, 65, 66, 67, 68,
                                   71, 72, 73, 74, 75, 76, 77, 78,
                                   81, 82, 83, 84, 85, 86, 87, 88,
                                   91, 92, 93, 94, 95, 96, 97, 98 }; // converts from a 64 based board to 120 squares based board.
                            
static const int sq120to64[120] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
                                    -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
                                    -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
                                    -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
                                    -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
                                    -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
                                    -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
                                    -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };// converts from a 120 squares based board to 64. 




 static const char files[120] =  { 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                                   'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'X',
                                   'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                                   'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'X' }; // converts form square number in the 120 sq base to file symbol.


static const int ranks[120] =     { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1,  1,  1,  1,  1,  1,  1,  1,  1, -1,
                                    -1,  2,  2,  2,  2 , 2,  2,  2,  2, -1,
                                    -1,  3,  3,  3,  3,  3,  3,  3,  3, -1,
                                    -1,  4,  4,  4,  4,  4,  4,  4,  4, -1,
                                    -1,  5,  5,  5,  5,  5,  5,  5,  5, -1,
                                    -1,  6,  6,  6,  6,  6,  6,  6,  6, -1,
                                    -1,  7,  7,  7,  7,  7,  7,  7,  7, -1,
                                    -1,  8,  8,  8,  8,  8,  8,  8,  8, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };                               
                                   

static const int pieceVals[12] = { 500, 300, 300, 900, 5000, 100, 500, 300, 300, 900, 5000, 100 }; // pieces point values, white and black.

static const int pieceVectors[6][8] = {   -10, 10, 1, -1, 0, 0, 0, 0, // rook
                                    -19, -21, 19, 21, -8, -12, 12, 8, // knight
                                    -9, -11, 11, 9, 0, 0, 0, 0, // bishop
                                    -10, 10, 1, -1, -9, 9, -11, 11, // queen
                                    -10, 10, 1, -1, -9, 9, -11, 11, // king
                                     0, 0, 0, 0, 0, 0, 0, 0   }; //the directions each of the 6 piece types can move in. pawn shows zero and gets handeled seperately
// mirrors the board
const static int flipBoard[64] =
{
	56,57,58,59,60,61,62,63,
	48,49,50,51,52,53,54,55,
	40,41,42,43,44,45,46,47,
	32,33,34,35,36,37,38,39,
	24,25,26,27,28,29,30,31,
	16,17,18,19,20,21,22,23,
	8, 9,10,11,12,13,14,15,
	0, 1, 2, 3, 4, 5, 6, 7
};

// mirrors pieces, ie: white queen will become black queen, etc. to be used in testing the evaluate functions.
const static int flipPiece[14] = { 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 12, 13 };
//{ WR, WN, WB, WQ, WK, WP, BR, BN, BB, BQ, BK, BP, EMPTY, OFFBOARD } 

// denotes if each piece is a slider or not, i.e runs over multiple squares
static const bool isSlider[6] = { true, false, true, true, false, false }; 

// returns the piece type, ie WN, BN = Knight, etc.
static const int pieceType[12] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN };

// victim scores, used for move ordering
static const int victimScores[12] = { 400, 200, 300, 500, 600, 100, 400, 200, 300, 500, 600, 100 };


















                                                   


 

                                                    

