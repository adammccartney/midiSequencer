SHELL=/bin/sh

CC = g++

CFLAGS = -w -std=c++17 
LIBS = -lgtest -lgmock -lpthread 

MKDIRP := mkdir -p
OUTDIR := build

OBJNAME = testMidiSequencer
OBJS = testMidiSequencer.cpp

.PHONY: directories all clean

all: directories $(OBJNAME) 

directories: $(OUTDIR)

$(OUTDIR):
	$(MKDIRP) $(OUTDIR)

$(OBJNAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(OUTDIR)/$(OBJNAME) $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OUTDIR) error.log
