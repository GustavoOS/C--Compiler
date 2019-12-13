ANALYSIS = obj/lex.yy.o obj/parser.o obj/util.o obj/memory.o obj/analyze.o obj/symtab.o 
SYNTHESIS = obj/codegenerator.o obj/syntaticErrors.o obj/instructions.o obj/library.o obj/mifgenerator.o
HELPER = obj/bytes.o
CC = g++
FLAGS = -Wall -g -std=c++11

cminus: $(ANALYSIS) $(SYNTHESIS) $(HELPER)
	$(CC) ${FLAGS} -o bin/cminus obj/*  main.cc -ly -lfl

obj/lex.yy.o: scanner.l
	flex -o scanner.c scanner.l
	$(CC)  -g -std=c++11 -c scanner.c -o obj/lex.yy.o

obj/util.o: util.cc
	$(CC) $(FLAGS) -c util.cc -o obj/util.o

obj/parser.o: cminus.y cminus.tab.c
	bison -d cminus.y
	$(CC) $(FLAGS) -c cminus.tab.c -o obj/parser.o

obj/codegenerator.o: codegenerator.cc
	$(CC) $(FLAGS) -c codegenerator.cc -o obj/codegenerator.o

obj/instructions.o: instructions.cc
	$(CC) $(FLAGS) -c instructions.cc -o obj/instructions.o

obj/memory.o: memory.cc
	$(CC) $(FLAGS) -c memory.cc -o obj/memory.o

obj/analyze.o: analyze.cc
	$(CC) $(FLAGS) -c analyze.cc -o obj/analyze.o

obj/syntaticErrors.o: syntaticErrors.cc
	$(CC) $(FLAGS) -c syntaticErrors.cc -o obj/syntaticErrors.o
		
obj/symtab.o: symtab.cc
	$(CC) $(FLAGS) -c symtab.cc -o obj/symtab.o

obj/library.o: library.cc
	$(CC) $(FLAGS) -c library.cc -o obj/library.o

obj/mifgenerator.o: mifgenerator.cc
	$(CC) $(FLAGS) -c mifgenerator.cc -o obj/mifgenerator.o

obj/bytes.o: bytes.cc
	$(CC) $(FLAGS) -c bytes.cc -o obj/bytes.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
