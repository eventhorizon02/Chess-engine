
#ifndef __Board__
#define __Board__
#include "defs.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>


using namespace std;

struct s_hist
{
	int bits;
	int side;
	int enPasSq;
	int hm_clock;
	char cast_rghts;
	uint64_t hashKey;
};

class Board
{
public:
	Board();
	~Board(){};
	void initBoard();
	void printBoard();
	void printColors(); // print piece colors occupying board
	void printBitboard(int64_t bitboard); // print the bits in a bitboard, for testing only.
	void setBoard(const string& FEN_String);
	void remakeBoard(); // set up board from bitboards after move was made.
	bool extractCastRgt (int place) const;
	void setCastRgt (int place, bool on); // sets a particular casteling right to on or off
	void mirror(); // mirrors the board and is used in testing the evaluation functions.
	uint64_t hashKey; // a unique key for the board position.
	int turn;
	int enPasSq; // the en passnt square in the 120 base.
	int hm_clock;  //This is the number of halfmoves since the last capture or pawn advance, it's used to determine a stalmate in accordace with the 50 move rule.
	int fm_number; // The number of the full move. It starts at 1, and is incremented after Black's move.
	char _cast_rgts; // will use bitwise to store casteling rights KQkq in that order
	char _color[64];
	vector <int> pList [12]; // holds the square numbers of each piece on the board.
	int64_t bitboards[12];
	int _board[120];
	int getRank(int sq_num); // returns the rank number for a given square number on the 120 sq board.
	int getFile(int sq_num); // returns the file number for a given square number on the 120 sq board.
	bool isSqAttacked(int sq, int side) const; // return a bool weather a certian square is attacked by side.
	bool isInCheck(int side) const; // is side in check
    uint64_t genHashKey();// generates a unique to the position 64 bit hash key.
    vector <s_hist> board_hist; // holds history of board data.
    vector <s_hist> hist_stack; // same as above to store history for take back and move forward.
    // some arrays to store the masks used to detect passed pawns and isolated pawns.
     uint64_t w_pawn_masks[64];
     uint64_t b_pawn_masks[64];
     uint64_t iso_pawn_masks[8];
     uint64_t single_file_masks[8]; // this one is to be used to detecd open files and semi open files for rooks and queens.
     int mvvlva[12][12]; // this array holds the score values for move ordering in the MVV/LVA fashion.
	
private:
     void parseFEN(const string& FEN_String);
     void setBitboardsAndColors();
     void initHashArrays(); //initiallises the hash arrays with random numbers, to be used in generating the unique hash position key
     void initMaskArrays(); // initialses the bit masks array used in detecting a passed pawn in the Eval class
     void initMvvLvaArrays(); // init the array that holds the ordering scores of the moves in the MVV/LVA fashion.

     //hash numbers storage arrays, needed to produce a unique hash key.
     uint64_t pieces_hashkey[12][64];
     uint64_t enPassant_hash[64];
     uint64_t castlePerm_hash[16];
     uint64_t side_hashKey; 

     
 };

#endif 













