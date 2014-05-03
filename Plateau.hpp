#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include <iostream>
#include <random>
#include <vector>

class Plateau
{
	private:

		int m_taille;
		int m_nbCombos;
		int m_tirsRestants;
		std::vector<std::vector<int>> m_grille;
		
	public:

		Plateau(int taille = 4);
		~Plateau();
		void initialiser();
		void augmenterNiveauCase(int x, int y);
		void exploserCase(int x, int y);
		void afficher();
		bool resolu() const;

		void augmenterTirsRestants() { ++m_tirsRestants; }
		void diminuerTirsRestants() { --m_tirsRestants; }
		void resetCombos() { m_nbCombos = 0; }

		int getTaille() const { return m_taille; }
		int getNiveauCase(int x, int y) const { return m_grille[x][y]; }
		int getNbCombos() const { return m_nbCombos; }
		int getTirsRestants() const { return m_tirsRestants; }
};

#endif

