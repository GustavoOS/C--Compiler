
bison: scanner.o
	gcc $(CFLAGS) -o cminus obj/* main.c -ly -lfl
	cminus

scanner.o:
	flex -o scanner.c scanner.l
	gcc $(CFLAGS) -c scanner.c -o obj/lex.yy.o

cminus.o:
	bison -d $(BFLAGS) cminus.y
	gcc $(CFLAGS) -c cminus.tab.c -o obj/cminus.o

view:
	dot -Tps calc.dot -o graph.ps; evince graph.ps
