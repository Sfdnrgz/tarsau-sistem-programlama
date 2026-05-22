CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude
TARGET=tarsau
SRC=src/main.c src/archive.c src/extract.c src/utils.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.sau
	rm -rf output d1

Project Updated
