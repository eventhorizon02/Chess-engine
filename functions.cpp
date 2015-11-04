#include "functions.h"

// set a particular bit to 1

 void setBit(int64_t *p_bitboard, int index)
{
  *p_bitboard |= (1LL << index);
}

// set a bit to 0 (first set it to 1 then xor with 1 to get 0)
 void clearBit(int64_t *p_bitboard, int index)
{
	*p_bitboard |= (1LL << index);
	*p_bitboard ^= (1LL << index);
}

// return a value of a bit
 bool retreiveBit(int64_t *p_bitboard, int index)
{
	return *p_bitboard & (1LL << index);
}

//generate a 64 bit random number to be used for hash keys
 uint64_t genRand64()
 {
  uint64_t hash = 0LL;
  hash |= ((uint64_t)rand() & 0x7fff);
  hash |= ((uint64_t)rand() & 0x7fff) << 15;
  hash |= ((uint64_t)rand() & 0x7fff) << 30;
  hash |= ((uint64_t)rand() & 0x7fff) << 45;
  hash |= ((uint64_t)rand() & 0xf)    << 60;
  return hash;
}


// debug functions

 // print binary
 void printBinary(const int dec)
 {
 	if (dec == 0)
 		return;
 	printBinary(dec / 2);

 	cout << dec % 2;
 }

  // return a string representing the move in algebraic, it take a move in the bit field int format and ignores most bit execpet to, from and promoted.
 string returnAlgebraic(const int bits)
 {
   string move;
   move.resize(5);
   move[0] = files[ GET_FROM (bits) ] + ' ';
   move[1] = '0' + ranks[ GET_FROM (bits) ];
   move[2] = files[ GET_TO (bits) ] + ' ';
   move[3]= '0' + ranks[ GET_TO (bits) ];
   int promotion = ( (bits >> 18) & 0xf );
   if ( (promotion >= WR) && (promotion <= BP) )
    move[4] = promo_symbols [promotion];
  
  return move;
 }

 // return the time in millisecons
int returnMillisecs()
{
	#ifdef WIN32
	return GetTickCount();
	#else
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
	#endif
	
}

 bool InputWaiting()
{
#ifndef WIN32
  fd_set readfds;
  struct timeval tv;
  FD_ZERO (&readfds);
  FD_SET (fileno(stdin), &readfds);
  tv.tv_sec=0; tv.tv_usec=0;
  select(16, &readfds, 0, 0, &tv);

  return (FD_ISSET(fileno(stdin), &readfds));
#else
   static int init = 0, pipe;
   static HANDLE inh;
   DWORD dw;

   if (!init) {
     init = 1;
     inh = GetStdHandle(STD_INPUT_HANDLE);
     pipe = !GetConsoleMode(inh, &dw);
     if (!pipe) {
        SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
        FlushConsoleInputBuffer(inh);
      }
    }
    if (pipe) {
      if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
      return dw;
    } else {
      GetNumberOfConsoleInputEvents(inh, &dw);
      return dw <= 1 ? 0 : dw;
	}
#endif
}

// read input from keyboard or GUI
void ReadInput(s_searchParamas* params)
{
	int  bytes;
    char input[256] = "", *endc;

    if (InputWaiting()) 
    {    
		params->stopped = true;
		do 
		{
		  bytes=read(fileno(stdin),input,256);
		} while (bytes<0);
		endc = strchr(input,'\n');
		if (endc) *endc=0;

		if (strlen(input) > 0)
		 {
			if (!strncmp(input, "quit", 4))
			 params->quit = true;
			   
		}
		return;
    }
}
 






























