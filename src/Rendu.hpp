#ifndef RENDU_HPP
#define RENDU_HPP

#include <irrlicht/irrlicht.h>
#include "Partie.hpp"
#include <string>
#include <array>
#include <list>
#include <deque>

enum typeNoeud :irr::s32
{
	CASE,
	SPHERE
};

enum etatJeu
{
	EN_JEU,
	PERDU
};

struct Sphere
{
	irr::scene::IAnimatedMeshSceneNode* node = nullptr;
	irr::scene::ISceneNodeAnimator* animator = nullptr;
};

struct MiniSphere
{
	irr::scene::IAnimatedMeshSceneNode* node = nullptr;
	irr::scene::ISceneNodeAnimator* animatorVol = nullptr;
	irr::scene::ISceneNodeAnimator* animatorDelete = nullptr;
	irr::scene::ISceneNodeAnimatorCollisionResponse* animatorCollision = nullptr;
};


class Rendu : public irr::IEventReceiver, public irr::scene::ICollisionCallback 		//La classe est également event handler
{
	private:

		irr::IrrlichtDevice* m_device;
		irr::video::IVideoDriver* m_driver;
		irr::scene::ISceneManager* m_sceneManager;

		//Éléments de GUI
		irr::gui::IGUIStaticText* m_tirsRestants;
		irr::gui::IGUIStaticText* m_niveau;

		Partie m_partie; 														//Pointeur vers le plateau de jeu représenté

		irr::scene::IAnimatedMesh* m_wumpa;										//Mesh de la sphère
		irr::scene::ISceneNode* m_pereCases;									//Hiérarchise le graphe de scène
		irr::scene::ISceneNode* m_pereSpheres;									//Hiérarchise le graphe de scène
		
		std::vector<std::vector<irr::scene::ISceneNode*>> m_casePlateau;		//Tableau des cases du plateau
		std::vector<std::vector<Sphere>> m_sphere; 								//Tableau des sphères

		irr::scene::ISceneNode* m_clickedSphere;								//Sphère cliquée

		std::list<irr::scene::ISceneNodeAnimator*> m_spheresEnChargement;
		std::list<MiniSphere> m_spheresEnVol;									//Sphères en cours de vol
		std::array <MiniSphere, 4> m_miniSphere;								//Stockage temporaire des mini sphères pour explosion.
		irr::scene::IMetaTriangleSelector* m_metaSelector;						//Sélecteur de sphère pour collision

		int m_etapeChargement;													//Représente l'état d'avancement de l'animation de début de niveau.
		etatJeu m_etatJeu;														//En cours de jeu ou perdu

	public:

		Rendu();
		~Rendu();

		irr::IrrlichtDevice* getDevice() const { return m_device; }
		irr::video::IVideoDriver* getDriver() const { return m_driver; }
		irr::scene::ISceneManager* getSceneManager() const { return m_sceneManager; }
		irr::gui::IGUIStaticText* getTirsRestants() const { return m_tirsRestants; }

		void dessinerPlateau();
		void dessinerSphere(int, int);
		void chargerSpheres();
		void clear();
		void testAnimator();

		void afficher();
		
		virtual bool OnEvent(const irr::SEvent &event);
		virtual bool onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);

		void rafraichir();
		void majSphere();
		void augmenterNiveauSphere(int, int);
		void exploserSphere(int, int);
		void lancer();
		void rendre();
		void majCombos();
		void recommencer();


		irr::core::vector3df calculDestinationMiniSphere(int, int, directionSphere);
};

#endif
