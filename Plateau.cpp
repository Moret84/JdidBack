#include "Plateau.hpp"

using namespace std;

Plateau::Plateau(int taille)
{
	m_taille = taille;
	m_grille.resize(taille); 

	for(int i = 0; i < taille; ++i)
		m_grille[i].resize(taille);

	initialiser();
}

Plateau::~Plateau()
{

}

void Plateau::initialiser()
{
/*	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> valeurCase(0, m_taille-1);

	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			m_grille[i][j]= valeurCase(gen);
			*/

	//DEBUG
/*	m_grille[0][0] = 3; m_grille[1][0] = 2; m_grille[2][0] = 1; m_grille[3][0] = 0;
	for(int i = 0; i < 4; ++i)
		for(int j = 1; j < 4; ++j)
			m_grille[i][j] = 0;*/

	m_grille[0][0] = 2;	
	m_grille[0][1] = 2;
	m_grille[0][2] = 2;
	m_grille[0][3] = 3;
	m_grille[1][0] = 1;
	m_grille[1][1] = 1;
	m_grille[1][2] = 2;
	m_grille[1][3] = 1;
	m_grille[2][0] = 0;
	m_grille[2][1] = 1;
	m_grille[2][2] = 1;
	m_grille[2][3] = 2;
	m_grille[3][0] = 3;
	m_grille[3][1] = 2;
	m_grille[3][2] = 2;
	m_grille[3][3] = 2;
}

void Plateau::afficher()
{
	for(int i = 0; i < m_taille; ++i)
	{
		for(int j =  0; j < m_taille; ++j)
			cout<<m_grille[i][j]<<" ";
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

bool Plateau::resolu() const
{
	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			if(m_grille[i][j] != 0)
				return false;
	return true;
}
