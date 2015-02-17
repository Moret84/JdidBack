CXX=g++
CXXFLAGS= -std=c++11 -Wall -ggdb -I ~/irrlicht/include
LDFLAGS= -lIrrlicht -lGL
ifeq ($(shell whoami),uapv1201349)
	LDFLAGS+=-L ~/irrlicht/lib -lXxf86vm -lX11
endif

SRCDIR=src
INCDIR=.
OBJDIR=obj

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXEC=Jeu

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

clean:
	@rm -f $(OBJ)

mrproper:clean
	@rm -f $(EXEC)

rebuild:mrproper all

.PHONY: clean mrproper rebuild
