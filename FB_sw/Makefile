CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -O2

TARGET = flappy_bird
OBJS = flappy_bird.c

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
