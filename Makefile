CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wshadow -pedantic
SOURCE = strace

all:
	$(CC) $(SOURCE).c -o $(SOURCE) $(CFLAGS)
