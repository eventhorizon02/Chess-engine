all:
	g++ main.cpp Board.cpp functions.cpp data.cpp IOCTRL.cpp MoveGen.cpp moveMaker.cpp Perft.cpp Eval.cpp SearchMv.cpp -o EGM
	