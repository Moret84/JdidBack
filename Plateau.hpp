#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include <iostream>
#include <random>
#include <vector>

class Plateau
{
	private:

		int m_taille;
		std::vector<std::vector<int>> m_grille;
		
	public:

		Plateau(int taille = 4);
		~Plateau();
		void initialiser();
		void augmenterNiveauCase(int x, int y);
		void exploserCase(int x, int y);
		void afficher();
		bool resolu() const;

		int getTaille() const { return m_taille; };
		int getNiveauCase(int x, int y) const { return m_grille[x][y]; }

};



#endif

