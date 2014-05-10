XX=g++
CXXFLAGS= -std=c++11 -Wall -ggdb
LDFLAGS= -lGL -lIrrlicht

SRCDIR=.
INCDIR=.
OBJDIR=obj

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(SRC:%.cpp=$(OBJDIR)/%.o)
EXEC=Jeu

all: $(EXEC) 

$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	@rm -f $(OBJ)

mrproper:clean
	@rm -f $(EXEC)

rebuild:mrproper all

.PHONY: clean mrproper rebuild
