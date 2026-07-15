CC = gcc
CFLAGS = -Wall
SRC = src/main.c 
OUTPUT = main 
LIBS = 


all: $(TARGET)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) $(LIBS) 

run: $(TARGET)
	./($TARGET)

clean: 
	rm -f $(TARGET)


.PHONY: all run clean
