cminus: scanner.o util.o memory.o analyze.o symtab.o codegenerator.o syntaticErrors.o
	g++ -Wall -g -std=c++11 -o bin/cminus obj/*  main.c -ly -lfl

scanner.o: parser.o
	flex -o scanner.c scanner.l
	g++  -g -std=c++11 -c scanner.c -o obj/lex.yy.o

util.o:
	g++ -g -Wall -std=c++11 -c util.c -o obj/util.o

parser.o:
	bison -d cminus.y
	g++ -g -Wall -std=c++11 -c cminus.tab.c -o obj/parser.o

codegenerator.o:
	g++ -g -Wall -std=c++11 -c codegenerator.cc -o obj/codegenerator.o

memory.o:
	g++ -g -Wall -std=c++11 -c memory.cc -o obj/memory.o

analyze.o:
	g++ -g -Wall -std=c++11 -c analyze.cc -o obj/analyze.o

syntaticErrors.o:
	g++ -g -Wall -std=c++11 -c syntaticErrors.cc -o obj/syntaticErrors.o
		
symtab.o:
	g++ -g -c -Wall -std=c++11 symtab.cc -o obj/symtab.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
