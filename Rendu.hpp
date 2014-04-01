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

class Rendu : public irr::IEventReceiver 										//La classe est également event handler
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
		std::queue<irr::scene::ISceneNodeAnimator*> m_animator;
		std::queue<irr::scene::IAnimatedMeshSceneNode*> m_miniSphere;

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
		void afficher();
		inline std::vector<irr::core::vector3df> calculPositionMiniSpheres(int, int);
		irr::scene::ISceneNodeAnimator* creerAnimateurSphere(int, int, directionSphere);
		inline irr::core::vector3df getPositionPremiereSphere(int, int, directionSphere);
		void testAnimator();
};

#endif
