# Makefile for lab1
# Programers Mark Zachacz, Ilya Novichenko
OBJ = main.o keywords.o yylex.o y.tab.o arraytype.o constant.o recordtype.o scope.o simpletype.o \
  symboltable.o stdidents.o variable.o pointertype.o parsefuncs.o bintree.o procfunc.o \
  settype.o symbol.o
H = y.tab.h yylex.h keywords.h
HSYMBOLS = arraytype.h constant.h recordtype.h simpletype.h variable.h type.h

a.out: $(OBJ) $(H)
	g++ -g $(OBJ)
	
main.o: main.cpp
	g++ -c -g main.cpp

yylex.o: $(H) yylex.cpp
	g++ -c -g yylex.cpp

keywords.o: keywords.h keywords.cpp y.tab.h
	g++ -c -g keywords.cpp
	
y.tab.o: y.tab.c
	g++ -c -g y.tab.c
	
y.tab.c y.tab.h: parse.y $(H) $(HSYMBOLS)
	yacc -d -v parse.y
	
symboltable.o: symboltable.h symboltable.cpp scope.h stdidents.h $(HSYMBOLS)
	g++ -c -g symboltable.cpp

stdidents.o: stdidents.h stdidents.cpp simpletype.h procfunc.h
	g++ -c -g stdidents.cpp

arraytype.o: arraytype.cpp arraytype.h type.h stdidents.h
	g++ -c -g arraytype.cpp

constant.o: constant.cpp constant.h symbol.h stdidents.h
	g++ -c -g constant.cpp

variable.o: variable.cpp variable.h symbol.h stdidents.h
	g++ -c -g variable.cpp

procfunc.o: procfunc.cpp procfunc.h scope.h simpletype.h
	g++ -c -g procfunc.cpp

scope.o: scope.cpp scope.h bintree.h
	g++ -c -g scope.cpp

bintree.o: bintree.cpp bintree.h symbol.h
	g++ -c -g bintree.cpp

pointertype.o: pointertype.cpp pointertype.h type.h
	g++ -c -g pointertype.cpp

parsefuncs.o: parsefuncs.cpp parsefuncs.h constant.h yylex.h symboltable.h symbol.h
	g++ -c -g parsefuncs.cpp

symbol.o: symbol.cpp symbol.h
	g++ -c -g symbol.cpp

settype.o: settype.cpp settype.h type.h
	g++ -c -g settype.cpp

recordtype.o: recordtype.cpp recordtype.h variable.h stdidents.h type.h
	g++ -c -g recordtype.cpp

clean:
	rm $(OBJ) a.out y.tab.c y.tab.h

.PHONY: clean