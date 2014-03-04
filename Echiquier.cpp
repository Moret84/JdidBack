#include "Echiquier.hpp"

Echiquier::Echiquier(int taille)
{
	m_taille = taille;
	m_cases = new int*[taille];

	for(int i = 0; i < taille; ++i)
		m_cases[i] = new int[taille];
}

Echiquier::~Echiquier()
{
	for(int i=0; i < m_taille; ++i)
		delete[] m_cases[i];
	delete[] m_cases;
}

void Echiquier::initialiser()
{
	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			m_cases[i][j]= -1;
}






