cminus: scanner.o util.o analyze.o symtab.o
	gcc -g -o bin/cminus obj/*  main.c -ly -lfl

scanner.o: parser.o
	flex -o scanner.c scanner.l
	gcc -g -c scanner.c -o obj/lex.yy.o

util.o:
	gcc -g -c util.c -o obj/util.o

parser.o:
	bison -d cminus.y
	gcc -g -c cminus.tab.c -o obj/parser.o

analyze.o:
	gcc -g -c analyze.c -o obj/analyze.o

symtab.o:
	gcc -g -c symtab.c -o obj/symtab.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
