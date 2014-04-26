
OPTS= -std=c++11 -Wall -lGL -lIrrlicht -ggdb
OBJ=obj/

all: Jeu

Jeu:main.cpp $(OBJ)Partie.o $(OBJ)Plateau.o $(OBJ)Rendu.o 
	g++ $(OPTS) -o Jeu $(OBJ)Partie.o $(OBJ)Plateau.o $(OBJ)Rendu.o main.cpp

$(OBJ)Plateau.o:Plateau.cpp
	g++ $(OPTS) -o $(OBJ)Plateau.o -c Plateau.cpp

$(OBJ)Rendu.o:Rendu.cpp 
	g++ $(OPTS) -o $(OBJ)Rendu.o -c Rendu.cpp

$(OBJ)Partie.o:Partie.cpp
	g++ $(OPTS) -o $(OBJ)Partie.o -c Partie.cpp

clean:
	rm -f $(OBJ)/*.o
