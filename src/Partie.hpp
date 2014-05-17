#ifndef PARTIE_HPP
#define PARTIE_HPP

#include <iostream>
#include <random>
#include <vector>

enum directionSphere : int 
{
	NORD = 0,
	SUD,
	EST,
	OUEST
};

class Partie
{
	private:

		int m_taille;
		int m_nbCombos;
		int m_tirsRestants;
		int m_niveau;
		std::vector<std::vector<int>> m_grille;
		
		void exploserCase(int , int);

	public:

		Partie(int taille = 4);
		~Partie();
		void initialiser();
		void choixCaseConsole(int, int);
		void choixCase(int, int);
		void afficher() const;
		void jouer();
		void majCombos();
		void levelUp();
		bool resolu() const;

		void augmenterTirsRestants() { ++m_tirsRestants; }
		void diminuerTirsRestants() { --m_tirsRestants; }
		void resetCombos() { m_nbCombos = 0; }
		int getTaille() const { return m_taille; }
		int getNiveauCase(int x, int y) const { return m_grille[x][y]; }
		int getNbCombos() const { return m_nbCombos; }
		int getTirsRestants() const { return m_tirsRestants; }
		int getNiveauPartie() const { return m_niveau; }
};

#endif

