#include "Echiquier.hpp"

Echiquier::Echiquier(int taille)
{
	m_taille = taille;
	m_cases = new Case*[taille];

	for(int i = 0; i < taille; ++i)
		m_cases[i] = new Case[taille];
}

Echiquier::~Echiquier()
{
	for(int i=0; i < m_taille; ++i)
		delete[] m_cases[i];
	delete[] m_cases;
}

void Echiquier::initialiser()
{




