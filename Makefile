CC = gcc 
CFLAGS = -Wall -Wextra -Wpadded -std=c11 -O3
LIBS = -lglfw -lGL

#Only specifiying the .o so .gitignore detects it. Otherwise its useless.
TARGET = main.o

SRC = src/main.c 

all: run 
run: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
.PHONY: all run 

