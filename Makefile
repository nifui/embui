CC = gcc 
CFLAGS = -Wall -Wextra -Wpadded -std=c11 -Os -v
LIBS = -lglfw -lGL
INCLUDE = -I/home/johnnywu/Documents/embui/include
#Only specifiying the .o so .gitignore detects it. Otherwise its useless.
TARGET = main.o

SRC = src/main.c 

all: run 
run: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS) $(INCLUDE)
.PHONY: all run 

