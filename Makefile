cminus: obj/lex.yy.o obj/parser.o obj/util.o obj/memory.o obj/analyze.o obj/symtab.o obj/codegenerator.o obj/syntaticErrors.o obj/instructions.o obj/library.o
	g++ -Wall -g -std=c++11 -o bin/cminus obj/*  main.c -ly -lfl

obj/lex.yy.o: scanner.l
	flex -o scanner.c scanner.l
	g++  -g -std=c++11 -c scanner.c -o obj/lex.yy.o

obj/util.o: util.cc
	g++ -g -Wall -std=c++11 -c util.cc -o obj/util.o

obj/parser.o: cminus.y cminus.tab.c
	bison -d cminus.y
	g++ -g -Wall -std=c++11 -c cminus.tab.c -o obj/parser.o

obj/codegenerator.o: codegenerator.cc
	g++ -g -Wall -std=c++11 -c codegenerator.cc -o obj/codegenerator.o

obj/instructions.o: instructions.cc
	g++ -g -Wall -std=c++11 -c instructions.cc -o obj/instructions.o

obj/memory.o: memory.cc
	g++ -g -Wall -std=c++11 -c memory.cc -o obj/memory.o

obj/analyze.o: analyze.cc
	g++ -g -Wall -std=c++11 -c analyze.cc -o obj/analyze.o

obj/syntaticErrors.o: syntaticErrors.cc
	g++ -g -Wall -std=c++11 -c syntaticErrors.cc -o obj/syntaticErrors.o
		
obj/symtab.o: symtab.cc
	g++ -g -c -Wall -std=c++11 symtab.cc -o obj/symtab.o

obj/library.o: library.cc
	g++ -g -c -Wall -std=c++11 library.cc -o obj/library.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
