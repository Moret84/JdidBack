CC=g++
CXXFLAGS= -std=c++11 -Wall -ggdb
LDFLAGS= -lGL -lIrrlicht

SRCDIR=.
OBJDIR=obj

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXEC=Jeu

all: $(EXEC) 

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: ../%.cpp
	$(CC) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -f $(OBJ)

mrproper:clean
	rm -f $(EXEC)


