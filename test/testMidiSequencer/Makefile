OBJS = testMidiSequencer.cpp

CC = g++
CFLAGS = -w -std=c++17
LIBS = -lgtest -lgmock -lpthread 

OBJ_NAME = testMidiSequencer

add: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(OBJ_NAME) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ_NAME) error.log
