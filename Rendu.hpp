#ifndef RENDU_HPP
#define RENDU_HPP

#include <irrlicht/irrlicht.h>
#include "Plateau.hpp"

enum typeNoeud :irr::s32 
{
	CASE,
	SPHERE
};

class Rendu : public irr::IEventReceiver
{
	private:

		irr::IrrlichtDevice* m_device;
		irr::video::IVideoDriver* m_driver;
		irr::scene::ISceneManager* m_sceneManager;

		Plateau* m_plateauRendu; 

		irr::scene::ISceneNode* m_pereCases;
		irr::scene::ISceneNode* m_pereSpheres;
		irr::scene::ISceneNode*** m_casePlateau;
		irr::scene::IAnimatedMeshSceneNode*** m_sphere;

		irr::scene::ISceneNode* m_clickedSphere;

	public:

		Rendu(Plateau * plateauRendu);
		~Rendu();

		irr::IrrlichtDevice* getDevice();
		irr::video::IVideoDriver* getDriver();
		irr::scene::ISceneManager* getSceneManager();
		void dessinerPlateau();
		void dessinerSpheres();			
		virtual bool OnEvent(const irr::SEvent &event);
		void majSphere();
};

#endif
