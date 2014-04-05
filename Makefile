
OPTS= -std=c++11 -Wall -lGL -lIrrlicht -ggdb

all: Jeu clean

Jeu:main.cpp Partie.o Plateau.o Rendu.o Collision.o
	g++ $(OPTS) -o Jeu Partie.o Plateau.o Rendu.o Collision.o main.cpp

Plateau.o:Plateau.cpp
	g++ $(OPTS) -o Plateau.o -c Plateau.cpp

Rendu.o:Rendu.cpp
	g++ $(OPTS) -o Rendu.o -c Rendu.cpp

Partie.o:Partie.cpp
	g++ $(OPTS) -o Partie.o -c Partie.cpp

Collision.o:Collision.cpp
	g++ $(OPTS) -o Collision.o -c Collision.cpp

clean:
	rm -f *.o
