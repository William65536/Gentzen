CC = clang
FLAGS = -Wall -Wextra -Wpedantic -Werror -std=c17
OBJ = Project/includes/Image.o Project/includes/LinearAlgebra.o Project/includes/IntervalList.o Project/Renderer/Solid.o

# TODO: Clean out needless files

main: Project/main.c $(OBJ)
	$(CC) $(FLAGS) -o Project/main $(OBJ) Project/main.c -lm

Image: Project/includes/Image.c
	$(CC) -c $(FLAGS) -o Project/includes/Image.o Project/includes/Image.c

LinearAlgebra: Project/includes/LinearAlgebra.c
	$(CC) -c $(FLAGS) -o Project/includes/LinearAlgebra.o Project/includes/LinearAlgebra.c

IntervalList: Project/includes/List.c
	$(CC) -c $(FLAGS) -DT=Interval -o Project/includes/IntervalList.o Project/includes/List.c

Solid: Project/Renderer/Solid.c
	$(CC) -c $(FLAGS) -o Project/Renderer/Solid.o Project/Renderer/Solid.c
