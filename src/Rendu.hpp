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
		irr::gui::IGUIFont* m_font;
		irr::gui::IGUIStaticText* m_tirsRestants;

		Partie m_partie; 														//Pointeur vers le plateau de jeu représenté

		irr::scene::IAnimatedMesh* m_wumpa;										//Mesh de la sphère
		irr::scene::ISceneNode* m_pereCases;									//Hiérarchise le graphe de scène
		irr::scene::ISceneNode* m_pereSpheres;									//Hiérarchise le graphe de scène
		
		std::vector<std::vector<irr::scene::ISceneNode*>> m_casePlateau;		//Tableau des cases du plateau
		std::vector<std::vector<Sphere>> m_sphere; //Tableau des sphères

		irr::scene::ISceneNode* m_clickedSphere;								//Sphère cliquée

		std::list<irr::scene::ISceneNodeAnimator*> m_spheresEnChargement;
		std::list<MiniSphere> m_spheresEnVol;									//Sphères en cours de vol
		std::array <MiniSphere, 4> m_miniSphere;								//Stockage temporaire des mini sphères pour explosion.
		irr::scene::IMetaTriangleSelector* m_metaSelector;						//Sélecteur de sphère pour collision

		int m_etapeChargement;

	public:

		Rendu();
		~Rendu();

		irr::IrrlichtDevice* getDevice() const { return m_device; }
		irr::video::IVideoDriver* getDriver() const { return m_driver; }
		irr::scene::ISceneManager* getSceneManager() const { return m_sceneManager; }
		irr::gui::IGUIFont* getGUIFont() const { return m_font; }
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


		irr::core::vector3df calculDestinationMiniSphere(int, int, directionSphere);
};

#endif
