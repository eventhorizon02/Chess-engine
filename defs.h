#ifndef __defs__
#define __defs__
#include "functions.h"
#include "data.cpp"
#define BRD_SQS_NUM 120 // number of squares on the borard.
#define NO_SQ -1
#define OPENING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TEST_POSITION "r5rk/1pp1qpn1/p2p3p/3P4/2PQNP1b/1P3B1P/P6K/5RR1 w - - 0 31"
#define AFTER_C5_MOVE "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define AFTER_NF3_MOVE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define CASTLE_1 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define CASTLE_2 "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define PROMO_WHITE "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
#define MATE_IN_TWO "6r1/5Q2/1n1p2pB/4k2b/3b3r/8/1NBRp2N/1K2R3 w - - 0 1"
#define MATE_IN_BLACK "8/8/8/1k6/3r4/8/1PP5/1KR2n2 b - - 0 1"

#define INF 1000000
#define MATE 999999
#define version 1.0
#define NO_MOVE -99

#define NDEBUG
#include <cassert>
enum { WR, WN, WB, WQ, WK, WP, BR, BN, BB, BQ, BK, BP, EMPTY, OFFBOARD };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
enum { WHITE, BLACK, NONE }; // who's turn it is or what piece color is occupying a certain square.
enum { BQC, BKC, WQC, WKC }; // casteling rights


// symbol used to print board and by FEN notations, they correspond to the index numbers of the pieces enum.                             
static char piece_symbols[14] = { 'R', 'N', 'B', 'Q', 'K', 'P', 'r', 'n', 'b', 'q', 'k', 'p', '.', '*' }; 
static char promo_symbols[14] = { 'r', 'n', 'b', 'q', 'k', 'p', 'r', 'n', 'b', 'q', 'k', 'p', '.', '*' };// promotion symbol for return algebraic for GUI

#define cordToSq(file,rank) sq64to120[ (rank*8) + file ]  //convert file and rank to a position on the 120 square board
#define GET_TO(b) ( b & 0x7f)
#define GET_FROM(b) ( (b >> 7) & 0x7f ) 
#define GET_CAPT_PCS(b) ( (b >> 14) & 0xf) // if it was a capture move, which piece was captured.
#define GET_PROMO_PCS(b) ( (b >> 18) & 0xf) // if it was a promotion move, which piece was the pawn promoted to.
#define IS_PROMO(b) ( (b >> 22) & 0x1 ) // was move a promotion
#define IS_EN_PASS_CAPT(b) ( (b >> 23) & 0x1 ) // was move an en-passant capture
#define IS_PAWN_START(b) ( (b >> 24) & 0x1 ) // was move a pawn start (2 squares)
#define IS_CASTLE(b) ( (b >> 25) & 0x1 ) // was it a castleing move.

// usefull squares
#define A1 21
#define B1 22
#define C1 23
#define D1 24
#define E1 25
#define F1 26
#define G1 27
#define H1 28

#define A8 91
#define B8 92
#define C8 93
#define D8 94
#define E8 95
#define F8 96
#define G8 97
#define H8 98

 

#endif









