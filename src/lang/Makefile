CC = cc -g
LEX = flex
YACC = bison
CFLAGS = -DYYDEBUG=1

PROGRAMS = test

all: ${PROGRAMS}

test: test.cpp parser.cpp scanner.cpp emit.cpp
	${CC} test.cpp emit.cpp parser.cpp scanner.cpp -lstdc++ -o $@


parser.cpp parser.hpp: grammar.y
	${YACC} $<

scanner.cpp scanner.hpp: lexer.l
	${LEX}  $<
