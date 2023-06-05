# TODO: Figure out how to clean files

CC = clang
FLAGS = -Wall -Wextra -Wpedantic -Werror -std=c17
OBJS = Project/includes/charList.o Project/Lexer/Lexer.o Project/includes/TokenList.o

main: Project/main.c $(OBJS)
	$(CC) $(FLAGS) -o Project/main $(OBJS) Project/main.c

charList: Project/includes/List.c
	$(CC) -c $(FLAGS) -DT=char -o Project/includes/charList.o Project/includes/List.c

TokenList: Project/includes/List.c
	$(CC) -c $(FLAGS) -DT=Token -o Project/includes/TokenList.o Project/includes/List.c

Lexer: Project/Lexer/Lexer.c
	$(CC) -c $(FLAGS) -o Project/Lexer/Lexer.o Project/Lexer/Lexer.c
