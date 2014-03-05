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
	m_grille[x][y] = 0;

	int i(0);

	//Vers le haut 
	i = y-1;
	while(i >= 0)
	{
		if(m_grille[x][i] != 0)
		{
			augmenterNiveauCase(x, i);
			break;
		}

		--i;
	}

	//Vers le bas
	i = y+1;
	while(i < m_taille)
	{
		if(m_grille[x][i] != 0)
		{
			augmenterNiveauCase(x, i);
			break;
		}

		++i;
	}

	//Vers la gauche
	i = x-1;
	while(i >=0)
	{
		if(m_grille[i][y] != 0)
		{
			augmenterNiveauCase(i, y);
			break;
		}

		--i;
	}

	//Vers la droite
	i = x+1;
	while(i < m_taille)
	{
		if(m_grille[i][y] != 0)
		{
			augmenterNiveauCase(i,y);
			break;
		}
		
		++i;
	}

}

bool Plateau::resolu()
{
	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			if(m_grille[i][j] != 0)
				return false;
	return true;
}

int Plateau::getTaille()
{
	return m_taille;
}
