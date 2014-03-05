#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include <iostream>
#include <random>

class Plateau
{
	private:

		int m_taille;
		int **m_grille;
		
	public:

		Plateau(int taille = 4);
		~Plateau();
		void initialiser();
		void augmenterNiveauCase(int x, int y);
		void exploserCase(int x, int y);
		void afficher();
		bool resolu();

		int getTaille();

};



#endif

