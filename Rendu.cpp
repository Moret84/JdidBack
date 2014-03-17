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

	//Debug FPS
	//m_sceneManager->addCameraSceneNodeFPS(0,100.0f,0.005f,-1);

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
	vector3df rotation(0,0,0);
	vector3df tailleWumpa[4], echelle, facteurPositionWumpa;
	//tailleWumpa = {vector3df(1.0f/300.0f, 0.01f,
	//tailleWumpa[3] = vector3df(0.01f, 0.01f, 0.01f); 
	tailleWumpa[3] = vector3df(1.0f,1.0f,1.0f);	
	tailleWumpa[2] = tailleWumpa[3] * (2.0f/3.0f);
	tailleWumpa[1] = tailleWumpa[3] / 3.0f;
		

	m_sphere = new IAnimatedMeshSceneNode**[m_taillePlateau];
	IAnimatedMesh* wumpa = m_sceneManager->getMesh("appletest.obj");

	for(int i = 0; i < m_taillePlateau; ++i)
	{
		m_sphere[i] = new IAnimatedMeshSceneNode*[m_taillePlateau];

		for(int j = 0; j < m_taillePlateau; ++j)
		{
			vector3df positionCase(m_casePlateau[i][j]->getPosition());
			vector3df positionSphere(positionCase.X , positionCase.Y + 0.11f, positionCase.Z);	//Place la sphère au dessus du plateau
			
			if(plateau[i][j] != 0)
			{
				echelle	= tailleWumpa[plateau[i][j]];
				m_sphere[i][j] = m_sceneManager->addAnimatedMeshSceneNode(wumpa, 0, -1, positionSphere, rotation, echelle);
				m_sphere[i][j]->setMaterialFlag(EMF_LIGHTING, false);
			}

		}
	}
}

Rendu::~Rendu()
{
	for(int i = 0; i < m_taillePlateau; ++i)
		for(int j = 0; j < m_taillePlateau; ++j)
		{
			delete m_casePlateau[i][j];
			delete m_sphere[i][j];
		}

	for(int i = 0; i < m_taillePlateau; ++i)
	{
		delete[] m_casePlateau[i];
		delete[] m_sphere[i];
	}
	
	delete[] m_casePlateau;
	delete[] m_sphere;
}



	


