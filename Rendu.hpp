#ifndef RENDU_HPP
#define RENDU_HPP

#include <irrlicht/irrlicht.h>
#include "Plateau.hpp"
#include <string>

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

class Rendu : public irr::IEventReceiver, irr::scene::ICollisionCallback						 				//La classe est également event handler
{
	private:

		irr::IrrlichtDevice* m_device;
		irr::video::IVideoDriver* m_driver;
		irr::scene::ISceneManager* m_sceneManager;

		Plateau* m_plateauRendu; 												//Pointeur vers le plateau de jeu repréenté 

		irr::scene::IAnimatedMesh * m_wumpa;									//Mesh de la sphère 
		irr::scene::ISceneNode* m_pereCases;									//Hiérarchise le graphe de scène 
		irr::scene::ISceneNode* m_pereSpheres;									//Hiérarchise le graphe de scène
		
		std::vector<std::vector<irr::scene::ISceneNode*>> m_casePlateau;		//Tableau des cases du plateau
		std::vector<std::vector<irr::scene::IAnimatedMeshSceneNode*>> m_sphere; //Tableau des sphères

		irr::scene::ISceneNode* m_clickedSphere;								//Sphère cliquée

///		CollisionHandler m_collisionHandler;

	public:

		Rendu(Plateau * plateauRendu);
		~Rendu();

		irr::IrrlichtDevice* getDevice() const { return m_device; } ;
		irr::video::IVideoDriver* getDriver() const { return m_driver; };
		irr::scene::ISceneManager* getSceneManager() const { return m_sceneManager; } ;

		void dessinerPlateau();
		void dessinerSpheres();			
		void afficher();

		virtual bool OnEvent(const irr::SEvent &event);
		virtual bool onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
		void majSphere();
		void augmenterNiveauSphere(int, int);
		void exploserSphere(int, int);

		inline std::vector<irr::core::vector3df> calculPositionMiniSpheres(int, int);
		inline irr::scene::ISceneNodeAnimator* creerAnimateurMiniSphere(int, int, directionSphere);
		inline irr::s32 getIdPremiereSphere(int, int, directionSphere);
};

#endif
