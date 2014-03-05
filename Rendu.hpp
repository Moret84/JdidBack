#ifndef RENDU_HPP
#define RENDU_HPP

#include <irrlicht/irrlicht.h>
class Rendu
{
	private:

		irr::IrrlichtDevice* m_device;
		irr::video::IVideoDriver* m_driver;
		irr::scene::ISceneManager* m_sceneManager;

		irr::scene::ISceneNode*** m_casePlateau;

	public:

		Rendu();

		irr::IrrlichtDevice* getDevice();
		irr::video::IVideoDriver* getDriver();
		irr::scene::ISceneManager* getSceneManager();
		void dessinerPlateau(int taille = 4);

};

#endif
