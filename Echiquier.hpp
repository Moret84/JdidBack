#ifndef ECHIQUIER_HPP
#define ECHIQUIER_HPP

class Echiquier
{
	private:

		int m_taille;
		int **m_cases;
		
	public:

		Echiquier(int taille);
		~Echiquier();
		void initialiser();
};



#endif

