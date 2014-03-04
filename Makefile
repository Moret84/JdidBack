
OPTS= -std=c++11 -lGL -lIrrlicht

all: Jeu clean

Jeu:main.cpp Partie.o Echiquier.o
	g++ $(OPTS) -o Jeu Partie.o Echiquier.o main.cpp

Echiquier.o:Echiquier.cpp
	g++ $(OPTS) -o Echiquier.o -c Echiquier.cpp

Partie.o:Partie.cpp
	g++ $(OPTS) -o Partie.o -c Partie.cpp

clean:
	rm -f *.o