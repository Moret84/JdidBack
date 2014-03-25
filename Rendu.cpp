#include "Rendu.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace std;

Rendu::Rendu(Plateau* plateauRendu)
{

	m_plateauRendu = plateauRendu;

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
	
	//Noeud père des cases du plateau
	m_pereCases = m_sceneManager->addEmptySceneNode(0, CASE);

	//Noeud père des cases du plateau
	m_pereSpheres = m_sceneManager->addEmptySceneNode(0, SPHERE);

	m_clickedSphere = nullptr;

	dessinerPlateau();
	dessinerSpheres();

	m_device->setEventReceiver(this);
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
	
	m_casePlateau = new ISceneNode**[m_plateauRendu->getTaille()];

	for(int i = 0; i < m_plateauRendu->getTaille(); ++i, x = 0.0f, z += 1.1f)
	{
		m_casePlateau[i] = new ISceneNode*[m_plateauRendu->getTaille()];

		for(int j = 0; j < m_plateauRendu->getTaille(); ++j, x+= 1.1f)
		{
			m_casePlateau[i][j] = m_sceneManager->addCubeSceneNode(
					1.0f, 											//Taille du cube
					m_pereCases, 									//Tous les cubes sont fils du noeud pereCases
					i * m_plateauRendu->getTaille() + j, 			//Calcul du numéro ID
					vector3df(x, y, z), 							//Position du cube, change à chaque tour de boucle
					vector3df(0, 0, 0), 							//Rotation, ici aucune
					vector3df(1.0f, 0.15f , 1.0f));					//Échelle, 0.15f en y pour une caisse fine en hauteur
			
			m_casePlateau[i][j]->setMaterialTexture(0, m_driver->getTexture("caisse.png"));
		}
	}
}

void Rendu::dessinerSpheres()
{
	vector3df tailleWumpa[4], echelle;
	tailleWumpa[3] = vector3df(1.0f,1.0f,1.0f);	
	tailleWumpa[2] = tailleWumpa[3] * (2.0f/3.0f);
	tailleWumpa[1] = tailleWumpa[3] / 3.0f;
		
	m_sphere = new IAnimatedMeshSceneNode**[m_plateauRendu->getTaille()];
	IAnimatedMesh* wumpa = m_sceneManager->getMesh("appletest.obj");

	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
	{
		m_sphere[i] = new IAnimatedMeshSceneNode*[m_plateauRendu->getTaille()]; 

		for(int j = 0; j < m_plateauRendu->getTaille(); ++j)
		{
			vector3df positionCase(m_casePlateau[i][j]->getPosition());							//Récupération position case courante
			vector3df positionSphere(positionCase.X , positionCase.Y + 0.11f, positionCase.Z);	//Place la sphère au dessus de cette case
			
			if(m_plateauRendu->getGrille()[i][j] != 0)
			{
				echelle	= tailleWumpa[m_plateauRendu->getGrille()[i][j]];		//Échelle calculée selon le niveau de la case

				m_sphere[i][j] = m_sceneManager->addAnimatedMeshSceneNode(
						wumpa, 											//Mesh chargé plus haut
						m_pereSpheres,									//Toutes les sphères sont filles de pereSpheres
					   	i * m_plateauRendu->getTaille() + j,			//Calcul du numéro ID 
					   	positionSphere,									//Position calculée
					   	vector3df(0,0,0),								//Rotation, ici aucune
					   	echelle);										//Echelle calculée 

				m_sphere[i][j]->setMaterialFlag(EMF_LIGHTING, false);
			}

			else
				m_sphere[i][j] = nullptr;
		}
	}
}

Rendu::~Rendu()
{
	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
		for(int j = 0; j < m_plateauRendu->getTaille(); ++j)
		{
			delete m_casePlateau[i][j];
			delete m_sphere[i][j];
		}

	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
	{
		delete[] m_casePlateau[i];
		delete[] m_sphere[i];
	}
	
	delete[] m_casePlateau;
	delete[] m_sphere;
}

bool Rendu::OnEvent(const SEvent &event)
{
	if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			position2d <s32> curseur = m_device
				->getCursorControl()
				->getPosition();															//Position du curseur au clic

			m_clickedSphere = m_sceneManager
				->getSceneCollisionManager()											
				->getSceneNodeFromScreenCoordinatesBB(curseur, 0, false, m_pereSpheres);	//Sphère en collision avec le clic

			return true;
		}	
	}
			
		return false;
}

void Rendu::majSphere()
{
	if(m_clickedSphere == nullptr)
		return;

	s32 i = m_clickedSphere->getID() / m_plateauRendu->getTaille();		
	s32 j = m_clickedSphere->getID() % m_plateauRendu->getTaille();

	IAnimatedMesh* wumpa = m_sceneManager->getMesh("appletest.obj");

	if(m_plateauRendu->getGrille()[i][j] ==	0)
	{
		vector3df positionCase(m_casePlateau[i][j]->getPosition());							//Récupération position case
		vector3df positionSphere(positionCase.X , positionCase.Y + 0.11f, positionCase.Z);	//Place la sphère au dessus du plateau

		m_sphere[i][j] = m_sceneManager->addAnimatedMeshSceneNode(
				wumpa,										//Mesh chargé plus haut                               
				m_pereSpheres,                              //Toutes les sphères sont filles de pereSpheres
			   	i * m_plateauRendu->getTaille() + j,        //Calcul du numéro ID 
			   	positionSphere,                             //Position de la sphère, fonction de celle de la case
			   	vector3df(0, 0, 0),                         //Rotation, ici aucune
			   	vector3df(1.0/3.0, 1.0/3.0, 1.0/3.0));      //Échelle, ici 1/3 car c'est une petite sphère qu'on ajoute 

		m_sphere[i][j]->setMaterialFlag(EMF_LIGHTING, false);
	
		m_plateauRendu->augmenterNiveauCase(i,j);
	}

	else if(m_plateauRendu->getGrille()[i][j] == 1)
	{
		m_sphere[i][j]
			->setScale(m_sphere[i][j]		
			->getScale() * 2.0);			

		m_plateauRendu->augmenterNiveauCase(i,j);
	}

	else if(m_plateauRendu->getGrille()[i][j] == 2)
	{
		m_sphere[i][j]
			->setScale(m_sphere[i][j]
			->getScale() * 3.0/2.0);

		m_plateauRendu->augmenterNiveauCase(i,j);
	}

	else if(m_plateauRendu->getGrille()[i][j] == 3)
	{
		m_sphere[i][j]->remove();
		m_sphere[i][j] = nullptr;

		vector3df positionCase(m_casePlateau[i][j]->getPosition());							//Récupération position case courante
		vector3df positionSphere[4];
		positionSphere[NORD] = vector3df(positionCase.X, positionCase.Y + 0.11f, positionCase.Z - 0.20f);
		positionSphere[SUD] = vector3df(positionCase.X, positionCase.Y + 0.11f, positionCase.Z + 0.20f );
		positionSphere[EST] = vector3df(positionCase.X + 0.20f , positionCase.Y + 0.11f, positionCase.Z);
		positionSphere[OUEST] = vector3df(positionCase.X - 0.20f, positionCase.Y + 0.11f, positionCase.Z);

		IAnimatedMeshSceneNode** miniSphere = new IAnimatedMeshSceneNode*[4];	

		for(s32 i = NORD; i <= OUEST; ++i)
		{

			miniSphere[i] = m_sceneManager->addAnimatedMeshSceneNode(
					wumpa,										//Mesh chargé plus haut                               
					0,                              			//Pas de père car vouée à disparaître
					-1,        									//Pas besoin d'ID non plus
					positionSphere[i],                          
					vector3df(0, 0, 0),                         //Rotation, ici aucune
					vector3df(1.0/3.0, 1.0/3.0, 1.0/3.0));      //Échelle, ici 1/3 car c'est une petite sphère qu'on ajoute 
			
			miniSphere[i]->setMaterialFlag(EMF_LIGHTING, false);
			
		}
	}
			

		m_clickedSphere = nullptr;
		return;
}
