#ifndef RENDU_HPP
#define RENDU_HPP

#include <irrlicht/irrlicht.h>
#include "Plateau.hpp"

class Rendu
{
	private:

		irr::IrrlichtDevice* m_device;
		irr::video::IVideoDriver* m_driver;
		irr::scene::ISceneManager* m_sceneManager;

		int m_taillePlateau;
		irr::scene::ISceneNode*** m_casePlateau;
		irr::scene::IAnimatedMeshSceneNode*** m_sphere;

	public:

		Rendu(int taille);
		~Rendu();

		irr::IrrlichtDevice* getDevice();
		irr::video::IVideoDriver* getDriver();
		irr::scene::ISceneManager* getSceneManager();
		void dessinerPlateau();
		void dessinerSpheres(int **plateau);			//On récupère l'état du plateau à l'instant t

};

#endif
