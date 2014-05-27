#include "Partie.hpp"

using namespace std;

Partie::Partie(int taille)
{
	m_taille = taille;
	m_grille.resize(taille);

	for(int i = 0; i < taille; ++i)
		m_grille[i].resize(taille);

	initialiser();

	m_nbCombos = 0;
	m_tirsRestants = 10;
	m_niveau = 1;
}

Partie::~Partie()
{

}

void Partie::initialiser()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> valeurCase(0, m_taille-1);

	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			m_grille[i][j]= valeurCase(gen);
			
}

void Partie::afficher() const
{
	cout<<"Nombre de tirs Restants : "<< m_tirsRestants<<endl;
	for(int i = 0; i < m_taille; ++i)
	{
		for(int j =  0; j < m_taille; ++j)
			cout<<m_grille[i][j]<<" ";
		cout<<endl;
	}
	cout<<"Niveau : "<<m_niveau<<endl;
}

void Partie::choixCaseConsole(int x, int y)
{
	--m_tirsRestants;

	if(m_grille[x][y] == 3)
		exploserCase(x, y);

	else
		++m_grille[x][y];
}

void Partie::choixCase(int x, int y)
{
	if(m_grille[x][y] == 3)
	{
		++m_nbCombos;
		m_grille[x][y] = 0;
	}
	
	else
		++m_grille[x][y];
}

void Partie::exploserCase(int x, int y)
{
	++m_nbCombos;
	m_grille[x][y] = 0;

	int i(0);

	//Vers le haut
	i = y-1;
	while(i >= 0)
	{
		if(m_grille[x][i] != 0)
		{
			if(m_grille[x][i] == 3)
				exploserCase(x, i);
			else
				++m_grille[x][i];

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
			if(m_grille[x][i] == 3)
				exploserCase(x, i);
			else
				++m_grille[x][i];
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
			if(m_grille[i][y] == 3)
				exploserCase(i, y);
			else
				++m_grille[i][y];
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
			if(m_grille[i][y] == 3)
				exploserCase(i, y);
			else
				++m_grille[i][y];
			break;
		}
		
		++i;
	}
}

bool Partie::resolu() const
{
	for(int i = 0; i < m_taille; ++i)
		for(int j = 0; j < m_taille; ++j)
			if(m_grille[i][j] != 0)
				return false;
	return true;
}

void Partie::majCombos()
{
	if(m_nbCombos >= 2)
		m_tirsRestants += m_nbCombos - 1;
	m_nbCombos = 0;
}

void Partie::jouer()
{
	int l, c;
	bool fin = false;
	while(!fin)
	{
		while(!resolu() && m_tirsRestants != 0)
		{
			afficher();
			cout<<"ligne ?"<<endl;
			cin>>l;
			cout<<"colonne ?"<<endl;
			cin>>c;
			choixCaseConsole(l, c);
			majCombos();
		}

		if(m_tirsRestants == 0)
		{
			cout<<"Perdu !"<<endl;
			fin = true;
		}

		if(resolu())
		{
			cout<<"Niveau Gagné !!"<<endl;
			++m_niveau;
			++m_tirsRestants;
			initialiser();
		}
	}
}

void Partie::levelUp()
{
	if(resolu())
	{
		++m_niveau;
		++m_tirsRestants;
		initialiser();
	}
}

void Partie::recommencer()
{
	initialiser();
	m_nbCombos = 0;
	m_tirsRestants = 10;
	m_niveau = 1;
}
