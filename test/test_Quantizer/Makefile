OBJS = Quantizer.cpp

CC = g++
CFLAGS = -w -std=c++14

OBJ_NAME = quantizer

add: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(OBJ_NAME)

.PHONY: clean
clean:
	rm -f $(OBJ_NAME) error.log
