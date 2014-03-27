#ifndef RENDU_HPP
#define RENDU_HPP

#include <irrlicht/irrlicht.h>
#include "Plateau.hpp"

enum typeNoeud :irr::s32 
{
	CASE,
	SPHERE
};

enum directionSphere :irr::s32
{
	NORD = 0,
	SUD,
	EST,
	OUEST
};

class Rendu : public irr::IEventReceiver										//La classe est également event handler
{
	private:

		irr::IrrlichtDevice* m_device;
		irr::video::IVideoDriver* m_driver;
		irr::scene::ISceneManager* m_sceneManager;

		Plateau* m_plateauRendu; 												//Pointeur vers le plateau rendu
		irr::scene::ISceneNode* m_pereCases;									//Hiérarchise le graphe de scène 
		irr::scene::ISceneNode* m_pereSpheres;									//Hiérarchise le graphe de scène
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
		void augmenterNiveauSphere(int, int);
		void exploserSphere(int, int);
		void majSphere();
		
		irr::core::vector3df getDestination(int , int , irr::s32);
};

#endif
