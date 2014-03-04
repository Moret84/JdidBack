#include "Plateau.hpp"

using namespace std;

Plateau::Plateau(int taille)
{
	m_taille = taille;
	m_grille = new int*[taille];

	for(int i = 0; i < taille; ++i)
		m_grille[i] = new int[taille];

	initialiser();
}

Plateau::~Plateau()
{
	for(int i=0; i < m_taille; ++i)
		delete[] m_grille[i];
	delete[] m_grille;
}

void Plateau::initialiser()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> valeurCase(0, m_taille-1);

	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			m_grille[i][j]= valeurCase(gen);
}

void Plateau::afficher()
{
	for(int i = 0; i < m_taille; ++i)
	{
		for(int j = 0; j < m_taille; ++j)
			cout<<m_grille[j][i]<<" ";
		cout<<endl;
	}
}

void Plateau::augmenterNiveauCase(int x, int y)
{
	if(m_grille[x][y] >= 3)
		exploserCase(x, y);

	else
		++m_grille[x][y];
}

void Plateau::exploserCase(int x, int y)
{

}

bool Plateau::resolu()
{
	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			if(m_grille[i][j] != 0)
				return false;
	return true;
}

