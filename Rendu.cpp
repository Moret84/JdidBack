#include "Rendu.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

Rendu::Rendu()
{
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

void Rendu::dessinerPlateau(int taille)
{
	f32 x, y, z; 
	x = y = z = 0.0f;
	

	m_casePlateau = new ISceneNode**[taille];
	
	for(int i = 0; i < taille; ++i, x = 0.0f, z += 1.1f)
	{
		m_casePlateau[i] = new ISceneNode*[taille];

		for(int j = 0; j < taille; ++j, x+= 1.1f)
		{
			m_casePlateau[i][j] = m_sceneManager->addCubeSceneNode(1.0f, 0,-1, vector3df(x, y, z), vector3df(0, 0, 0), vector3df(1.0f, 0.15f, 1.0f));
			m_casePlateau[i][j]->setMaterialTexture(0, m_driver->getTexture("caisse.png"));
		}
	}
}


