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
	if(!m_clickedSphere)
		return;

	s32 i = m_clickedSphere->getID() / m_plateauRendu->getTaille();		
	s32 j = m_clickedSphere->getID() % m_plateauRendu->getTaille();

	augmenterNiveauSphere(i, j);

	m_clickedSphere = nullptr;
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
		}


		return;
}



void Rendu::exploserSphere(int x, int y)
{
	if(!m_sphere[x][y])
		return;

	std::vector<ISceneNode*> miniSphere(4);

	for(s32 k = NORD; k <= OUEST; ++k)
	{	
		miniSphere[k] = m_sceneManager->addAnimatedMeshSceneNode(
				m_wumpa, 
				0,
				x * m_plateauRendu->getTaille() + y,
				calculPositionMiniSpheres(x, y, (directionSphere) k),
				vector3df(0, 0, 0),
				vector3df(1.0/3.0, 1.0/3.0, 1.0/3.0));
		miniSphere[k]->setMaterialFlag(EMF_LIGHTING, false);
		ISceneNodeAnimator* animator(creerAnimateurVol(x, y, (directionSphere) k));
		miniSphere[k]->addAnimator(animator);
		animator->drop();
	}

	m_sceneManager->addToDeletionQueue(m_sphere[x][y]);
	m_sphere[x][y] = nullptr;
}

vector3df Rendu::calculPositionMiniSpheres(int x, int y, directionSphere direction)
{
	vector3df positionSphere(m_casePlateau[x][y]->getPosition());
	vector3df positionMiniSphere;

	switch(direction)
	{
		case NORD:
			positionMiniSphere = vector3df(positionSphere.X, positionSphere.Y + 0.11f, positionSphere.Z - 0.20f);
			break;
		case SUD:
			positionMiniSphere = vector3df(positionSphere.X, positionSphere.Y + 0.11f, positionSphere.Z + 0.20f );
			break;
		case EST:
			positionMiniSphere = vector3df(positionSphere.X - 0.20f , positionSphere.Y + 0.11f, positionSphere.Z);
			break;
		case OUEST:
			positionMiniSphere = vector3df(positionSphere.X + 0.20f, positionSphere.Y + 0.11f, positionSphere.Z);
			break;
	}

	return positionMiniSphere;

}

ISceneNode* Rendu::getPremiereSphere(int x, int y, directionSphere direction)
{
	int i;

	if(direction == OUEST)
	{
		i = y-1;
		while(i >= 0)
		{
			if(m_sphere[x][i])
				return m_sphere[x][i];

			--i;
		}
	}

	else if(direction == EST)
	{
		i = y+1;
		while(i < m_plateauRendu->getTaille())
		{
			if(m_sphere[x][i])
				return m_sphere[x][i];

			++i;
		}
	}

	else if(direction == NORD)
	{
		i = x-1;
		while(i >= 0)
		{
			if(m_sphere[i][y])
				return m_sphere[i][y];

			--i;
		}
	}

	else if(direction == SUD)
	{
		i = x+1;
		while(i < m_plateauRendu->getTaille())
		{
			if(m_sphere[i][y])
				return m_sphere[i][y];

			++i;
		}
	}

	return nullptr;
}
	
ISceneNodeAnimator* Rendu::creerAnimateurVol(s32 x, s32 y, directionSphere direction)
{
	ISceneNode* sphereDestination(getPremiereSphere(x, y, direction));
	vector3df positionMiniSphere(calculPositionMiniSpheres(x, y, direction)), arrivee(positionMiniSphere);

	if(!sphereDestination)
	{
		f32 distance;

		switch(direction)
		{
			case NORD:	
				arrivee.Z = -m_casePlateau[0][0]->getScale().Z; 
				distance = abs_(arrivee.Z - positionMiniSphere.Z);
				break;

			case SUD:
				arrivee.Z = m_plateauRendu->getTaille() * m_casePlateau[0][0]->getScale().Z;
				distance = abs_(arrivee.Z - positionMiniSphere.Z);
				break;

			case EST:
				arrivee.X = - m_casePlateau[0][0]->getScale().X;
				distance = abs_(arrivee.X - positionMiniSphere.X);
				break;

			case OUEST:
				arrivee.X = m_plateauRendu->getTaille() * m_casePlateau[0][0]->getScale().X; 
				break;
		}
		
		f32 vitesse = 0.002f;
		
		if(direction == NORD)
			cerr<<"NORD "<<arrivee.Z<<endl;

		else if(direction == SUD)
			cerr<<"SUD "<<arrivee.Z<<endl;

		else if(direction == EST)
			cerr<<"EST "<<arrivee.X<<endl;

		else if(direction == OUEST)
			cerr<<"OUEST "<<arrivee.X<<endl;
				
		return m_sceneManager->createFlyStraightAnimator(positionMiniSphere, arrivee, distance / vitesse);
	}

	else
	{
		vector3df positionSphereDestination(sphereDestination->getPosition());
		vector3df distance(positionSphereDestination - positionMiniSphere);
		if(direction == NORD || direction == SUD)
		{
				return m_sceneManager->createFlyStraightAnimator(positionMiniSphere, positionSphereDestination, abs_(distance.Z/0.004));
		}
		else
			return m_sceneManager->createFlyStraightAnimator(positionMiniSphere, positionSphereDestination, abs_(distance.X/0.004));
	}
}
