CC = gcc
FLAGS = -Wall -Werror -std=gnu99
APP = matrix_mult

$(APP): $(APP).c main.c
	$(CC) $(FLAGS) $^ -o $@ -lpthread

clean:
	rm -rf $(APP)
