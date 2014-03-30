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
	m_device = createDevice(EDT_OPENGL, dimension2d<u32>(640,480), 32, false, false, false, this);
	m_driver = m_device->getVideoDriver();
	m_sceneManager = m_device->getSceneManager();

	//Lumière ambiante
	m_sceneManager->setAmbientLight(SColorf(1.0,1.0,1.0,0.0));

	//Caméra fixe
	m_sceneManager->addCameraSceneNode(0, vector3df(1.6f, 3, 4.3f), vector3df(1.6f, 0, 2.2f));
	
	//Debug FPS
	//m_sceneManager->addCameraSceneNodeFPS(0,100.0f,0.005f,-1);

	//Chargement du mesh
	m_wumpa = m_sceneManager->getMesh("appletest.obj");
	
	//Noeud père des cases du plateau
	m_pereCases = m_sceneManager->addEmptySceneNode(0, CASE);

	//Noeud père des cases du plateau
	m_pereSpheres = m_sceneManager->addEmptySceneNode(0, SPHERE);

	m_clickedSphere = nullptr;

	m_casePlateau.resize(m_plateauRendu->getTaille());
	m_sphere.resize(m_plateauRendu->getTaille());

	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
	{
		m_casePlateau[i].resize(m_plateauRendu->getTaille());
		m_sphere[i].resize(m_plateauRendu->getTaille());
	}	

	dessinerPlateau();
	dessinerSpheres();
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

	for(int i = 0; i < m_plateauRendu->getTaille(); ++i, x = 0.0f, z += 1.1f)
	{
			for(int j = m_plateauRendu->getTaille() - 1; j >= 0; --j, x+= 1.1f)
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
		
	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
	{
		for(int j = 0; j < m_plateauRendu->getTaille(); ++j)
		{
			vector3df positionCase(m_casePlateau[i][j]->getPosition());							//Récupération position case courante
			vector3df positionSphere(positionCase.X , positionCase.Y + 0.11f, positionCase.Z);	//Place la sphère au dessus de cette case
			
			if(m_plateauRendu->getNiveauCase(i, j) != 0)
			{
				echelle	= tailleWumpa[m_plateauRendu->getNiveauCase(i, j)];		//Échelle calculée selon le niveau de la case

				m_sphere[i][j] = m_sceneManager->addAnimatedMeshSceneNode(
						m_wumpa, 											//Mesh chargé plus haut
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

void Rendu::augmenterNiveauSphere(int x, int y)
{
		/*if(m_plateauRendu->getNiveauCase(x, y) == 0)
		{
			vector3df positionCase(m_casePlateau[x][y]->getPosition()), positionSphere(positionCase.X, positionCase.Y + 0.11f, positionCase.Z);

			m_sphere[x][y] = m_sceneManager->addAnimatedMeshSceneNode(
					m_wumpa,												
					m_pereSpheres,
					x * m_plateauRendu->getTaille() + y,
					positionSphere,
					vector3df(0, 0, 0),
					vector3df(1.0/3.0, 1.0/3.0, 1.0/3.0));

			m_sphere[x][y]->setMaterialFlag(EMF_LIGHTING, false);
			m_plateauRendu->augmenterNiveauCase(x, y);
		}
			
		else*/ if(m_plateauRendu->getNiveauCase(x, y) == 1)
		{
			m_sphere[x][y]->setScale(
					m_sphere[x][y]->getScale() * 2.0);
			m_plateauRendu->augmenterNiveauCase(x,y);	
		}
		
		else if(m_plateauRendu->getNiveauCase(x, y) == 2)
		{
			m_sphere[x][y]->setScale(
						m_sphere[x][y]->getScale() * 3.0/2.0);
			m_plateauRendu->augmenterNiveauCase(x,y);
		}

		else if(m_plateauRendu->getNiveauCase(x, y) == 3)
		{
			exploserSphere(x, y);
			m_plateauRendu->augmenterNiveauCase(x,y);
		}
}

void Rendu::afficher()
{
	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
	{
		for(int j = 0; j < m_plateauRendu->getTaille(); ++j)
		{
			if(!m_sphere[i][j])
				cout<<"0 ";
			else
				cout<< m_sphere[i][j]->getScale().X << " ";
		}
		cout<<endl;
	}

	for(int i = 0; i < m_plateauRendu->getTaille(); ++i)
	{
		for(int j = 0; j < m_plateauRendu->getTaille(); ++j)
			cout<<"("<<m_casePlateau[i][j]->getPosition().X<<", "<<m_casePlateau[i][j]->getPosition().Z<<") ";
		cout<<endl;
	}
}

inline std::vector<vector3df> Rendu::calculPositionMiniSpheres(int x, int y)
{
	vector3df positionCase(m_casePlateau[x][y]->getPosition());
	std::vector<vector3df> positionMiniSphere(4);

	positionMiniSphere[NORD] = vector3df(positionCase.X, positionCase.Y + 0.11f, positionCase.Z - 0.20f);
	positionMiniSphere[SUD] = vector3df(positionCase.X, positionCase.Y + 0.11f, positionCase.Z + 0.20f );
	positionMiniSphere[EST] = vector3df(positionCase.X - 0.20f , positionCase.Y + 0.11f, positionCase.Z);
	positionMiniSphere[OUEST] = vector3df(positionCase.X + 0.20f, positionCase.Y + 0.11f, positionCase.Z);

	return positionMiniSphere;
}

void Rendu::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{

}


void Rendu::exploserSphere(int x, int y)
{
	if(!m_sphere[x][y])
		return;

	m_sphere[x][y]->remove();
	m_sphere[x][y] = nullptr;

	std::vector<vector3df> positionSphere(calculPositionMiniSpheres(x, y)), destinationSphere(4);
	std::vector<IAnimatedMeshSceneNode*> miniSphere(4);
	std::vector<ISceneNodeAnimator*> animatorSphere(4), animatorSphere(4);

	for(s32 k = NORD; k <= OUEST; ++k)
	{
		miniSphere[k] = m_sceneManager->addAnimatedMeshSceneNode(
				m_wumpa,											//Mesh chargé plus haut                               
				0,													//Pas de père car vouée à disparaître
				-1,													//Pas besoin d'ID non plus
				positionSphere[k],								
				vector3df(0, 0, 0),									//Rotation, ici aucune
				vector3df(1.0/3.0, 1.0/3.0, 1.0/3.0));				//Échelle, ici 1/3 car c'est une petite sphère qu'on ajoute 

		miniSphere[k]->setMaterialFlag(EMF_LIGHTING, false);
		animatorVolSphere[k] = creerAnimateurSphere(x, y, (directionSphere) k);
		miniSphere[k]->addAnimator(animatorSphere[k]);
	}



		/*vector3df test(getPositionPremiereSphere(x, y, NORD));
		cerr<<test.X<<", "<<test.Y<<", "<<test.Z<<endl;
		animatorSphere[NORD] = creerAnimateurSphere(x, y, (directionSphere) NORD);
		miniSphere[NORD]->addAnimator(animatorSphere[NORD]);*/
	
		/*for(s32 k = NORD; k <= OUEST; ++k)
		{
			if(animatorSphere[k]->hasFinished())
				miniSphere[k]->remove();
		}*/
}

inline vector3df Rendu::getPositionPremiereSphere(int x, int y, directionSphere direction)
{
	int i;

	if(direction == OUEST)
	{
		i = y-1;
		while(i >= 0)
		{
			if(m_sphere[x][i])
				return m_sphere[x][i]->getPosition();

			--i;
		}
	}

	else if(direction == EST)
	{
		i = y+1;
		while(i < m_plateauRendu->getTaille())
		{
			if(m_sphere[x][i])
				return m_sphere[x][i]->getPosition();

			++i;
		}
	}

	else if(direction == NORD)
	{
		i = x-1;
		while(i >= 0)
		{
			if(m_sphere[i][y])
				return m_sphere[i][y]->getPosition();

			--i;
		}
	}

	else if(direction == SUD)
	{
		i = x+1;
		while(i < m_plateauRendu->getTaille())
		{
			if(m_sphere[i][y])
				return m_sphere[i][y]->getPosition();

			++i;
		}
	}

	return vector3df(0.1,0.1,0.1);
}
	
void Rendu::majSphere()
{
	if(m_clickedSphere == nullptr)
		return;

	s32 i = m_clickedSphere->getID() / m_plateauRendu->getTaille();		
	s32 j = m_clickedSphere->getID() % m_plateauRendu->getTaille();

	augmenterNiveauSphere(i, j);

	m_clickedSphere = nullptr;
}

ISceneNodeAnimator* Rendu::creerAnimateurSphere(int x, int y, directionSphere direction)
{
	vector3df sphereDestination(getPositionPremiereSphere(x, y, direction));
	std::vector<vector3df> positionMiniSphere(calculPositionMiniSpheres(x, y)), distance(4, vector3df(0,0,0));

	if(sphereDestination == vector3df(0.1, 0.1, 0.1))
	{
		int taillePlateau(m_plateauRendu->getTaille()), tailleCase(m_casePlateau[0][0]->getScale().X);

		distance[NORD].Z = - ((taillePlateau - 1) * tailleCase + 0.8f);
		distance[SUD].Z = ((taillePlateau - 1) * tailleCase + 0.8f);
		distance[EST].X = - ((taillePlateau - 1) * tailleCase + 0.8f);
		distance[OUEST].X = ((taillePlateau - 1) * tailleCase + 0.8f);

		return m_sceneManager->createFlyStraightAnimator(positionMiniSphere[direction], positionMiniSphere[direction] + distance[direction], 1000);
	}

	else
	{
		vector3df distance(sphereDestination - positionMiniSphere[direction]);
		if(direction == NORD || direction == SUD)
		{
				return m_sceneManager->createFlyStraightAnimator(positionMiniSphere[direction], sphereDestination, abs_(distance.Z/0.004));
		}
		else
			return m_sceneManager->createFlyStraightAnimator(positionMiniSphere[direction], sphereDestination, abs_(distance.X/0.004));
	}
}
