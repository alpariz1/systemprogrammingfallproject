CC = gcc
CFLAGS = -Wall -Wextra -g -I/home/alper/include
LDFLAGS =

SRC =  unal3.c
OBJ = $(SRC:.c=.o)
TARGET = unal

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

