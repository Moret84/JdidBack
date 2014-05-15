#include "Rendu.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace std;
using namespace gui;

Rendu::Rendu() : m_partie()
{
	//Device avec API = OPENGL, Fenêtre de taille 640 x 480p et 32bits par pixel
	m_device = createDevice(EDT_OPENGL, dimension2d<u32>(800,600), 32, false, false, false, this);
	m_device->setWindowCaption(L"Jdidback");

	m_driver = m_device->getVideoDriver();
	m_sceneManager = m_device->getSceneManager();

	//Lumière ambiante
	m_sceneManager->setAmbientLight(SColorf(1.0,1.0,1.0,0.0));

	//Caméra fixe
	m_sceneManager->addCameraSceneNode(0, vector3df(1.6f, 4, 4.5f), vector3df(1.6f, 1, 1.0f));
	
	//Debug FPS
	//m_sceneManager->addCameraSceneNodeFPS(0,100.0f,0.005f,-1);

	//Chargement du mesh
	m_wumpa = m_sceneManager->getMesh("appletest.obj");
	
	//Noeud père des cases du plateau
	m_pereCases = m_sceneManager->addEmptySceneNode(0, CASE);

	//Noeud père des cases du plateau
	m_pereSpheres = m_sceneManager->addEmptySceneNode(0, SPHERE);

	//Sphère cliquée
	m_clickedSphere = nullptr;

	//Monde à prendre en compte pour les collisions
	m_metaSelector = m_sceneManager->createMetaTriangleSelector();

	m_casePlateau.resize(m_partie.getTaille());
	m_sphere.resize(m_partie.getTaille());

	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		m_casePlateau[i].resize(m_partie.getTaille());
		m_sphere[i].resize(m_partie.getTaille());
	}	

	m_sceneManager->addSkyDomeSceneNode(m_driver->getTexture("nsanitybeach.jpg"), 16, 8, 1, 10000000);
	m_font = m_device->getGUIEnvironment()->getFont("superwumpa.xml");
	m_tirsRestants = m_device->getGUIEnvironment()->addStaticText(stringw(m_partie.getTirsRestants()).c_str(), rect<s32>(5, 10, 105, 60));
	m_tirsRestants->setOverrideFont(m_font);
	m_tirsRestants->setOverrideColor(SColor(255, 234, 102, 0));

	dessinerPlateau();

	/*for(int i = 0; i < m_partie.getTaille(); ++i)
		for(int j = 0; j < m_partie.getTaille(); ++j)
			dessinerSphere(i, j);*/
	chargerSpheres();
}

bool Rendu::resolu() const
{
	for(int i = 0; i < m_partie.getTaille(); ++i)
		for(int j = 0; j < m_partie.getTaille(); ++j)
			if(m_sphere[i][j])
				return false;
	return true;
}

void Rendu::dessinerPlateau()
{
	f32 x, y, z;
	x = y = z = 0.0f;

	for(int i = 0; i < m_partie.getTaille(); ++i, x = 0.0f, z += 1.0f)
	{
		for(int j = m_partie.getTaille() - 1; j >= 0; --j, x+= 1.0f)
		{
			m_casePlateau[i][j] = m_sceneManager->addCubeSceneNode(
					1.0f, 											//Taille du cube
					m_pereCases, 									//Tous les cubes sont fils du noeud pereCases
					i * m_partie.getTaille() + j, 					//Calcul du numéro ID
					vector3df(x, y, z), 							//Position du cube, change à chaque tour de boucle
					vector3df(0), 									//Rotation, ici aucune
					vector3df(1.0f, 0.15f , 1.0f));					//Échelle, 0.15f en y pour une caisse fine en hauteur

			m_casePlateau[i][j]->setMaterialTexture(0, m_driver->getTexture("caisse.png"));
		}
	}
}

void Rendu::dessinerSphere(int x, int y)
{
	u32 niveauCase = m_partie.getNiveauCase(x, y);

	if(niveauCase != 0)
	{
		vector3df echelle;

		switch(niveauCase)
		{
			case 1:
				echelle = vector3df(1.0/3);
				break;
			case 2:
				echelle = vector3df(2.0/3);
				break;
			case 3:
				echelle = vector3df(1);
			break;
		}

		vector3df positionCase(m_casePlateau[x][y]->getPosition());
		vector3df positionSphere(positionCase.X , positionCase.Y + 0.11f, positionCase.Z);	//Place la sphère au dessus de cette case

		m_sphere[x][y] = m_sceneManager->addAnimatedMeshSceneNode(
				m_wumpa, 										//Mesh chargé plus haut
				m_pereSpheres,									//Toutes les sphères sont filles de pereSpheres
				x * m_partie.getTaille() + y,					//Calcul du numéro ID
				positionSphere,									//Position calculée
				vector3df(0),									//Rotation, ici aucune
				echelle);										//Echelle calculée

		m_sphere[x][y]->setMaterialFlag(EMF_LIGHTING, false);

		ITriangleSelector* selector = m_sceneManager->createTriangleSelectorFromBoundingBox(m_sphere[x][y]);
		m_sphere[x][y]->setTriangleSelector(selector);
		selector->drop();
		m_metaSelector->addTriangleSelector(m_sphere[x][y]->getTriangleSelector());

		vector3df positionDepart = m_sphere[x][y]->getPosition();
		//vector3df positionArrivee = vector3df(positionDepart.X , positionDepart.Y + 0.07, positionDepart.Z - 0.05);
		vector3df positionArrivee = vector3df(positionDepart.X , positionDepart.Y + 0.15, positionDepart.Z);

		ISceneNodeAnimator* animator = m_sceneManager->createFlyStraightAnimator(
				positionDepart,
				positionArrivee,
				500,
				true,
				true);

		m_sphere[x][y]->addAnimator(animator);
		animator->drop();

		animator = m_sceneManager->createRotationAnimator(vector3df(0, 3, 0));

		m_sphere[x][y]->addAnimator(animator);
		animator->drop();
	}

	else
		m_sphere[x][y] = nullptr;
}

void Rendu::chargerSpheres()
{
	vector3df positionDepart, positionArrivee;
	ISceneNodeAnimator *animatorChute, *animatorRebond;

	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
		{
			dessinerSphere(i, j);

			if(m_sphere[i][j])
			{
				positionDepart = m_casePlateau[i][j]->getPosition();
				positionDepart.Y += 10.0;
				positionArrivee = m_casePlateau[i][j]->getPosition();
				positionArrivee.Y += 0.11;
				animatorChute = m_sceneManager->createFlyStraightAnimator(positionDepart, positionArrivee, 500, false, false);
				animatorChute->drop();

				positionDepart = positionArrivee;
				positionArrivee.Y += (2 - 0.11);
				animatorRebond = m_sceneManager->createFlyStraightAnimator(positionDepart, positionArrivee, 500, true, false);
				animatorRebond->drop();

				//m_sphere[i][j]->addAnimator(animatorChute);
				//animatorChute->drop();
				//m_sphere[i][j]->addAnimator(animatorRebond);
				//animatorRebond->drop();
			}
		}
	}

	/*for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
		{
			if(m_sphere[i][j])
			{
				positionDepart = m_casePlateau[i][j]->getPosition();
				positionDepart.Y += 0.11;
				positionArrivee = positionDepart;
				positionArrivee.Y += (1.0 - 0.11);
				animator = m_sceneManager->createFlyStraightAnimator(positionDepart, positionArrivee,1000, false, true);
				m_sphere[i][j]->addAnimator(animator);
				animator->drop();
			}
		}
	}*/
}

Rendu::~Rendu()
{

}

void Rendu::afficher()
{
	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
		{
			if(!m_sphere[i][j])
				cout<<"0 ";
			else
				cout<< m_sphere[i][j]->getScale().X << " ";
		}
		cout<<endl;
	}

	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
			cout<<"("<<m_casePlateau[i][j]->getPosition().X<<", "<<m_casePlateau[i][j]->getPosition().Z<<") ";
		cout<<endl;
	}
}

bool Rendu::onCollision(const ISceneNodeAnimatorCollisionResponse &animator)
{
	int index = animator.getCollisionNode()->getID();
	int x = index / m_partie.getTaille();
	int y = index % m_partie.getTaille();
	m_sceneManager->addToDeletionQueue(animator.getTargetNode());
	
	augmenterNiveauSphere(x, y);
	
	return true;
}
void Rendu::testAnimator()
{
	auto it = m_fileAnimation.begin();
	while(it != m_fileAnimation.end())
	{
		if(it->animatorCollision->collisionOccurred() || it->animatorDelete->hasFinished())
			m_fileAnimation.erase(it++);

		else
			++it;
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

			if(!m_clickedSphere)
			{	
				m_clickedSphere = m_sceneManager
					->getSceneCollisionManager()											
					->getSceneNodeFromScreenCoordinatesBB(curseur, 0, false, m_pereCases);	//Case en collision avec le clic
			}

		}	
			return true;
	}
			
		return false;
}

void Rendu::majSphere()
{
	if(!m_fileAnimation.empty())
		m_clickedSphere = nullptr;

	if(!m_clickedSphere)
		return;
	
	m_tirsRestants->setText(stringw(m_partie.getTirsRestants()).c_str());

	s32 i = m_clickedSphere->getID() / m_partie.getTaille();		
	s32 j = m_clickedSphere->getID() % m_partie.getTaille();

	augmenterNiveauSphere(i, j);

	m_clickedSphere = nullptr;

}

void Rendu::augmenterNiveauSphere(int x, int y)
{

	if(!m_sphere[x][y])
	{
		m_partie.choixCase(x, y);
		dessinerSphere(x, y);
	}

	else if(m_sphere[x][y]->getScale() == vector3df(1.0/3))
	{
		m_sphere[x][y]->setScale(m_sphere[x][y]->getScale() * 2);
		m_partie.choixCase(x, y);	
	}

	else if(m_sphere[x][y]->getScale() == vector3df(2.0/3))
	{
		m_sphere[x][y]->setScale(m_sphere[x][y]->getScale() * 3.0/2);
		m_partie.choixCase(x, y);
	}

	else if(m_sphere[x][y]->getScale() == vector3df(1))
		exploserSphere(x, y);
}

void Rendu::exploserSphere(int x, int y)
{
	if(!m_sphere[x][y])
		return;

	vector3df destinationMiniSphere, positionMiniSphere;
	f32 tempsAnimation, vitesseAnimation(0.003f);

	for(s32 k = NORD; k <= OUEST; ++k)
	{	
		positionMiniSphere = m_sphere[x][y]->getPosition();
		destinationMiniSphere = calculDestinationMiniSphere(x, y, (directionSphere) k);

		if(k == NORD || k == SUD)
			tempsAnimation = abs_(destinationMiniSphere.Z - positionMiniSphere.Z) / vitesseAnimation;
		else
			tempsAnimation = abs_(destinationMiniSphere.X - positionMiniSphere.X) / vitesseAnimation;

		m_miniSphere[k].node = m_sceneManager->addAnimatedMeshSceneNode(
				m_wumpa,
				0,
				-1,
				positionMiniSphere,
				vector3df(0),
				vector3df(1.0/3));
		
		m_miniSphere[k].node->setMaterialFlag(EMF_LIGHTING, false);

		m_miniSphere[k].animatorVol = m_sceneManager->createFlyStraightAnimator(positionMiniSphere, destinationMiniSphere, tempsAnimation);

		//On retire la sphère venant d'exploser du sélecteur de triangle
		m_metaSelector->removeTriangleSelector(m_sphere[x][y]->getTriangleSelector());

		m_miniSphere[k].animatorCollision = m_sceneManager
			->createCollisionResponseAnimator(
					m_metaSelector,
					m_miniSphere[k].node,
					vector3df(1.0/3),
					vector3df(0));

		m_miniSphere[k].animatorCollision->setCollisionCallback(this);

		m_miniSphere[k].animatorDelete = m_sceneManager->createDeleteAnimator(tempsAnimation);

		m_fileAnimation.push_back(m_miniSphere[k]);
	}

	if(m_sphere[x][y])
	{
		m_sceneManager->addToDeletionQueue(m_sphere[x][y]);
		m_sphere[x][y] = nullptr;
	}

	m_partie.choixCase(x, y);

	for(auto it = m_miniSphere.begin(); it != m_miniSphere.end(); ++c,++it)
	{
		it->node->addAnimator(it->animatorVol);
		it->animatorVol->drop();
		
		if(it->animatorCollision)
			it->node->addAnimator(it->animatorCollision);

		if(it->animatorDelete)
			it->node->addAnimator(it->animatorDelete);
	}
}

vector3df Rendu::calculDestinationMiniSphere(s32 x, s32 y, directionSphere direction)
{
	vector3df arrivee(m_sphere[x][y]->getPosition());

	switch(direction)
	{
		case NORD:	
			arrivee.Z = - m_casePlateau[0][0]->getScale().Z;
			break;

		case SUD:
			arrivee.Z = m_partie.getTaille() * m_casePlateau[0][0]->getScale().Z;
			break;

		case EST:
			arrivee.X = - m_casePlateau[0][0]->getScale().X;
			break;

		case OUEST:
			arrivee.X = m_partie.getTaille() * m_casePlateau[0][0]->getScale().X;
			break;
	}

	return arrivee;
}

void Rendu::rendre()
{
	m_driver->beginScene(true, true, SColor(255, 255, 255, 255));
	m_sceneManager->drawAll();
	//cout<<R.getDriver()->getFPS()<<endl;
	//m_rendu.getDevice()->getGUIEnvironment()->drawAll();
	testAnimator();
	m_driver->endScene();
}

void Rendu::lancer()
{
	while(m_device->run())
	{
		majSphere();
		rendre();
	}
	m_device->drop();
}
