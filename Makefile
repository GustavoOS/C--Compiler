cminus: scanner.o util.o
	gcc -o bin/cminus obj/*  main.c -ly -lfl
	bin/cminus programa.c

scanner.o: parser.o
	flex -o scanner.c scanner.l
	gcc -c scanner.c -o obj/lex.yy.o

util.o:
	gcc -c util.c -o obj/util.o

parser.o:
	bison -d cminus.y
	gcc -c cminus.tab.c -o obj/parser.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
