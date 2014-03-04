
OPTS= -std=c++11 -lGL -lIrrlicht -ggdb

all: Jeu clean

Jeu:main.cpp Partie.o Plateau.o
	g++ $(OPTS) -o Jeu Partie.o Plateau.o main.cpp

Plateau.o:Plateau.cpp
	g++ $(OPTS) -o Plateau.o -c Plateau.cpp

Partie.o:Partie.cpp
	g++ $(OPTS) -o Partie.o -c Partie.cpp

clean:
	rm -f *.o
