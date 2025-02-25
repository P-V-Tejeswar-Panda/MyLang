all: BUILD/myLang/myLang.out

BUILD/myLang/myLang.out: BUILD/lexer/lex.yy.o BUILD/lexer/lexer.o BUILD/parser/parser.o BUILD/parser/expr.o BUILD/errors/errors.o BUILD/interpreter/interpreter.o BUILD/parser/stmt.o BUILD/environment/environment.o BUILD/myLang/main.o
	g++ -g -I src/include BUILD/lexer/lexer.o BUILD/lexer/lex.yy.o BUILD/parser/parser.o BUILD/parser/expr.o BUILD/errors/errors.o BUILD/interpreter/interpreter.o BUILD/parser/stmt.o BUILD/environment/environment.o BUILD/myLang/main.o -o BUILD/myLang/myLang.out
BUILD/myLang/main.o:
	g++ -g -I src/include -c src/myLang/main.cpp -o BUILD/myLang/main.o
BUILD/lexer/lex.yy.o: BUILD/lexer/lex.yy.c
	g++ -g -I src/include -c BUILD/lexer/lex.yy.c -o BUILD/lexer/lex.yy.o
BUILD/lexer/lexer.o:
	g++ -g -I src/include -c src/lexer/lexer.cpp -o BUILD/lexer/lexer.o
BUILD/lexer/lex.yy.c:
	flex -o BUILD/lexer/lex.yy.c src/lexer/lexer.l
BUILD/parser/parser.o:
	g++ -g -I src/include -c src/parser/parser.cpp -o BUILD/parser/parser.o
BUILD/parser/expr.o:
	g++ -g -I src/include -c src/parser/expr.cpp -o BUILD/parser/expr.o
BUILD/parser/stmt.o:
	g++ -g -I src/include -c src/parser/stmt.cpp -o BUILD/parser/stmt.o
BUILD/errors/errors.o:
	g++ -g -I src/include -c src/errors/errors.cpp -o BUILD/errors/errors.o
BUILD/interpreter/interpreter.o:
	g++ -g -I src/include -c src/interpreter/interpreter.cpp -o BUILD/interpreter/interpreter.o
BUILD/environment/environment.o:
	g++ -g -I src/include -c src/environment/environment.cpp -o BUILD/environment/environment.o

clean:
	rm -f BUILD/myLang/*
	rm -f BUILD/lexer/*
	rm -f BUILD/parser/*
	rm -f BUILD/errors/*
	rm -f BUILD/interpreter/*
	rm -f BUILD/environment/*