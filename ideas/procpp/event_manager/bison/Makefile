

design:
	bison --defines=design.tab.h design.ypp -o design.tab.cpp
	flex++ -o design.lex.cpp design.l
	g++ *.cpp -lfl -o design
