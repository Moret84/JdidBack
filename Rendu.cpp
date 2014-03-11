#include "Rendu.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

Rendu::Rendu(int taillePlateau)
{

	//Taille du plateau à rendre, on la stocke car on sait qu'elle ne changera pas après avoir lancé le rendu
	m_taillePlateau = taillePlateau;

	//Device avec API = OPENGL, Fenêtre de taille 640 x 480p et 32bits par pixel
	m_device = createDevice(EDT_OPENGL, dimension2d<u32>(640,480), 32);

	m_driver = m_device->getVideoDriver();
	m_sceneManager = m_device->getSceneManager();

	//Lumière ambiante
	m_sceneManager->setAmbientLight(SColorf(1.0,1.0,1.0,0.0));

	//Caméra fixe
	m_sceneManager->addCameraSceneNode(0, vector3df(1.6f, 3, 4.3f), vector3df(1.6f, 0, 2.2f));

	dessinerPlateau();
}

IrrlichtDevice* Rendu::getDevice()
{
	return m_device;
}

IVideoDriver* Rendu::getDriver()
{
	return m_driver;
}

ISceneManager* Rendu::getSceneManager()
{
	return m_sceneManager;
}

void Rendu::dessinerPlateau()
{
	f32 x, y, z; 
	x = y = z = 0.0f;
	

	m_casePlateau = new ISceneNode**[m_taillePlateau];
	
	for(int i = 0; i < m_taillePlateau; ++i, x = 0.0f, z += 1.1f)
	{
		m_casePlateau[i] = new ISceneNode*[m_taillePlateau];

		for(int j = 0; j < m_taillePlateau; ++j, x+= 1.1f)
		{
			m_casePlateau[i][j] = m_sceneManager->addCubeSceneNode(1.0f, 0,-1, vector3df(x, y, z), vector3df(0, 0, 0), vector3df(1.0f, 0.15f, 1.0f));
			m_casePlateau[i][j]->setMaterialTexture(0, m_driver->getTexture("caisse.png"));
		}
	}
}

void Rendu::dessinerSpheres(int** plateau)
{
	f32 x, y, z;

	m_sphere = new ISceneNode**[m_taillePlateau];

	for(int i = 0; i < m_taillePlateau; ++i)
	{
		m_sphere[i] = new ISceneNode*[m_taillePlateau];

		for(int j = 0; j < m_taillePlateau; ++j)
		{
		}
	}
}

	


