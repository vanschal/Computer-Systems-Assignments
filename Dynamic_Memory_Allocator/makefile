CC = gcc
FLAGS = -Wall -Werror -std=gnu99
APP = mem_alloc

$(APP): $(APP).c main.c
	$(CC) $(FLAGS) $^ -o $@

test: $(APP).c test_main.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -rf $(APP) test

