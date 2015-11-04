#ifndef __functions__
#define __functions__
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "defs.h"
#include <string>

using namespace std;

struct s_searchParamas
 {
    int starttime;
	int stoptime;
	int depth;
	int timeset;
	int movestogo;
	long nodes;
	bool quit;
	bool stopped;
};


// set a particular bit to 1

 void setBit(int64_t *p_bitboard, int index);


// set a bit to 0 (first set it to 1 then xor with 1 to get 0)
 void clearBit(int64_t *p_bitboard, int index);


// return a value of a bit
 bool retreiveBit(int64_t *p_bitboard, int index);

 //generate a 64 bit random number to be used for hash keys
 uint64_t genRand64();

 // debug functions

 // print binary
 void printBinary(const int dec);

 // return a string representing the move in algebraic, it take a move in the bit field int format and ignores most bit execpet to, from and promoted.
 string returnAlgebraic(const int bits);

 int returnMillisecs(); // return the number of milliseconds.
  bool InputWaiting(); // input waiting in the buffer from keyboard or GUI.
 void ReadInput(s_searchParamas* params); // read the input from buffer.

 
#endif


#ifdef WIN32
#include "windows.h"
#else
#include "sys/time.h"
#include "sys/select.h"
#include "unistd.h"
#endif





















