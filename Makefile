all: BUILD/lexer/lexer.out

BUILD/lexer/lexer.out: BUILD/lexer/lex.yy.c
	g++ -I src/include src/lexer/lexer.cpp BUILD/lexer/lex.yy.c -o BUILD/lexer/lexer.out

BUILD/lexer/lex.yy.c:
	flex -o BUILD/lexer/lex.yy.c src/lexer/lexer.l

clean:
	rm -f BUILD/lexer/lexer.out
	rm -f BUILD/lexer/lex.yy.c
