#ifndef PARTIE_HPP
#define PARTIE_HPP

#include <irrlicht/irrlicht.h>
#include "Rendu.hpp"

class Partie
{
	private : 
		
		Plateau	m_plateau;
		Rendu m_rendu;
	
	public:

		Partie(int taillePlateau = 4);
		void jouer();
		void sauver();
		void charger();

};

#endif
