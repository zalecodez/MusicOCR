# Makefile for CS 453
# compile binary from multiple source files

SRC = main.cpp smooth.cpp edges.cpp houghL.cpp houghC.cpp
BIN = hough

DBG = -g

IMGLIB = /home/schar/cs453/imageLib
ARCH := $(shell arch)

CC = g++
CPPFLAGS = -O2 -W -Wall $(DBG) -I$(IMGLIB)
LDLIBS = -L$(IMGLIB) -lImg.$(ARCH)$(DBG) -lpng -lz
OBJ = $(SRC:.cpp=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDLIBS)

clean: 
	rm -f $(BIN) $(OBJ)
