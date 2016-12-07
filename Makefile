cminus: scanner.o util.o analyze.o symtab.o
	gcc -o bin/cminus obj/*  main.c -ly -lfl

scanner.o: parser.o
	flex -o scanner.c scanner.l
	gcc -c scanner.c -o obj/lex.yy.o

util.o:
	gcc -c util.c -o obj/util.o

parser.o:
	bison -d cminus.y
	gcc -c cminus.tab.c -o obj/parser.o

analyze.o:
	gcc -c analyze.c -o obj/analyze.o

symtab.o:
	gcc -c symtab.c -o obj/symtab.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
