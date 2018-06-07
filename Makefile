cminus: scanner.o util.o memory.o analyze.o symtab.o codegenerator.o
	g++ -g -o bin/cminus obj/*  main.c -ly -lfl

scanner.o: parser.o
	flex -o scanner.c scanner.l
	g++ -g -c scanner.c -o obj/lex.yy.o

util.o:
	g++ -g -c util.c -o obj/util.o

parser.o:
	bison -d cminus.y
	g++ -g -c cminus.tab.c -o obj/parser.o

codegenerator.o:
	g++ -g -c codegenerator.cc -o obj/codegenerator.o

memory.o:
	g++ -g -c memory.cc -o obj/memory.o

analyze.o:
	g++ -g -c analyze.cc -o obj/analyze.o

	
symtab.o:
	g++ -g -c symtab.c -o obj/symtab.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
