all: BUILD/myLang/myLang.out

BUILD/myLang/myLang.out: BUILD/lexer/lex.yy.o BUILD/lexer/lexer.o BUILD/myLang/main.o
	g++ -I src/include BUILD/lexer/lexer.o BUILD/lexer/lex.yy.o BUILD/myLang/main.o -o BUILD/myLang/myLang.out
BUILD/myLang/main.o:
	g++ -I src/include -c src/myLang/main.cpp -o BUILD/myLang/main.o
BUILD/lexer/lex.yy.o: BUILD/lexer/lex.yy.c
	g++ -I src/include -c BUILD/lexer/lex.yy.c -o BUILD/lexer/lex.yy.o
BUILD/lexer/lexer.o:
	g++ -I src/include -c src/lexer/lexer.cpp -o BUILD/lexer/lexer.o
BUILD/lexer/lex.yy.c:
	flex -o BUILD/lexer/lex.yy.c src/lexer/lexer.l

clean:
	rm -f BUILD/myLang/*
	rm -f BUILD/lexer/*
