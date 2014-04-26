#include "Partie.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace std;

Partie::Partie(int taille) : m_plateau(taille), m_rendu(&m_plateau)
{

}

void Partie::jouer()
{
	video::SColor blanc(255,255,255,255);
	while (m_rendu.getDevice()->run())
	{
		m_rendu.getDriver()->beginScene(true,true, blanc);
		m_rendu.majSphere();
		//R.afficher();
	//	P.afficher();
//		cout<<R.getDriver()->getFPS()<<endl;
		m_rendu.getSceneManager()->drawAll();
		m_rendu.getDriver()->endScene();
		//P.afficher();
	}

	m_rendu.getDevice()->drop();
}
