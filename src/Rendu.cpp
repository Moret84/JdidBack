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
	m_wumpa = m_sceneManager->getMesh("media/mesh/apple.obj");

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

	m_sceneManager->addSkyDomeSceneNode(m_driver->getTexture("media/textures/nsanitybeach.jpg"), 16, 8, 1.0, 2.0, 1000.0, 0, -1);

	IGUIFont* policeTirsRestants = m_device->getGUIEnvironment()->getFont("media/fonts/superwumpa.xml");
	IGUIFont* policeNiveau = m_device->getGUIEnvironment()->getFont("media/fonts/superWumpaNiveau.xml");
	IGUIFont* fontMini = m_device->getGUIEnvironment()->getFont("media/fonts/superWumpaToutpetit.xml");

	SColor orangeBandicoot = SColor(255, 234, 102, 0);

	m_device->getGUIEnvironment()->getSkin()->setFont(fontMini);
	m_device->getGUIEnvironment()->getSkin()->setColor(EGDC_BUTTON_TEXT, orangeBandicoot);
	m_device->getGUIEnvironment()->getSkin()->setColor(EGDC_ACTIVE_CAPTION, orangeBandicoot);
	m_device->getGUIEnvironment()->getSkin()->setColor(EGDC_INACTIVE_CAPTION, orangeBandicoot);

	m_tirsRestants = m_device->getGUIEnvironment()->addStaticText(stringw(m_partie.getTirsRestants()).c_str(), rect<s32>(5, 10, 105, 80));
	m_tirsRestants->setOverrideFont(policeTirsRestants);

	IGUIStaticText*	labelNiveau = m_device->getGUIEnvironment()->addStaticText(stringw(L"Niveau").c_str(), rect<s32>(650, 30, 750, 80));
	labelNiveau->setOverrideFont(policeNiveau);
	m_niveau = m_device->getGUIEnvironment()->addStaticText(stringw(m_partie.getNiveauPartie()).c_str(), rect <s32>(760, 30, 800, 80));
	m_niveau->setOverrideFont(policeNiveau);

	dessinerPlateau();

	m_etapeChargement = 0;
	chargerSpheres();

	m_etatJeu = EN_JEU;
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

			m_casePlateau[i][j]->setMaterialTexture(0, m_driver->getTexture("media/textures/caisse.png"));
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

		m_sphere[x][y].node = m_sceneManager->addAnimatedMeshSceneNode(
				m_wumpa, 										//Mesh chargé plus haut
				m_pereSpheres,									//Toutes les sphères sont filles de pereSpheres
				x * m_partie.getTaille() + y,					//Calcul du numéro ID
				positionSphere,									//Position calculée
				vector3df(0),									//Rotation, ici aucune
				echelle);										//Echelle calculée

		m_sphere[x][y].node->setMaterialFlag(EMF_LIGHTING, false);

		ITriangleSelector* selector = m_sceneManager->createTriangleSelectorFromBoundingBox(m_sphere[x][y].node);
		m_sphere[x][y].node->setTriangleSelector(selector);
		selector->drop();
		m_metaSelector->addTriangleSelector(m_sphere[x][y].node->getTriangleSelector());

		//Rotation
		ISceneNodeAnimator* animator;
		animator = m_sceneManager->createRotationAnimator(vector3df(0, 3, 0));
		m_sphere[x][y].node->addAnimator(animator);
		animator->drop();
	}

	else
		m_sphere[x][y].node = nullptr;
}

void Rendu::chargerSpheres()
{
	if(m_etapeChargement > 1)
		m_etapeChargement = -1;

	if(m_etapeChargement == -1)
		return;

	vector3df positionDepart, positionArrivee;

	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
		{
			if(m_etapeChargement == 0)
				dessinerSphere(i, j);

			if(m_sphere[i][j].node)
			{
				//Chute
				if(m_etapeChargement == 0)
				{
					positionDepart = m_casePlateau[i][j]->getPosition();
					positionDepart.Y += 10.0;
					positionArrivee = m_casePlateau[i][j]->getPosition();
					positionArrivee.Y += 0.11;
					m_sphere[i][j].animator = m_sceneManager->createFlyStraightAnimator(positionDepart, positionArrivee, 500, false, false);

					m_spheresEnChargement.push_back(m_sphere[i][j].animator);
					m_sphere[i][j].node->addAnimator(m_sphere[i][j].animator);
				}

				//Lévitation
				else if(m_etapeChargement == 1)
				{
					positionDepart = m_sphere[i][j].node->getPosition();
					positionArrivee = positionDepart;
					positionArrivee.Y += 0.15f;

					m_sphere[i][j].node->removeAnimator(m_sphere[i][j].animator);
					m_sphere[i][j].animator = m_sceneManager
						->createFlyStraightAnimator(positionDepart, positionArrivee, 500, true, true);
				}

				m_sphere[i][j].node->addAnimator(m_sphere[i][j].animator);
			}
		}
	}
	++m_etapeChargement;
}

Rendu::~Rendu()
{

}

void Rendu::clear()
{
	m_metaSelector->removeAllTriangleSelectors();
	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
		{
			if(m_sphere[i][j].node)
			{
				m_sceneManager->addToDeletionQueue(m_sphere[i][j].node);
				m_sphere[i][j].node = nullptr;
			}
		}
	}
}

void Rendu::afficher()
{
	for(int i = 0; i < m_partie.getTaille(); ++i)
	{
		for(int j = 0; j < m_partie.getTaille(); ++j)
		{
			if(!m_sphere[i][j].node)
				cout<<"0 ";
			else
				cout<< m_sphere[i][j].node->getScale().X << " ";
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
	if(m_spheresEnVol.empty() && m_spheresEnChargement.empty())
		return;

	if(m_spheresEnChargement.empty())
	{
		auto it = m_spheresEnVol.begin();
		while(it != m_spheresEnVol.end())
		{
			if(it->animatorCollision->collisionOccurred() || it->animatorDelete->hasFinished())
				m_spheresEnVol.erase(it++);

			else
				++it;
		}
	}

	else
	{
		auto it = m_spheresEnChargement.begin();
		while(it != m_spheresEnChargement.end())
		{
				if((*it)->hasFinished())
					m_spheresEnChargement.erase(it++);

				else
					++it;
		}
	}
}

bool Rendu::OnEvent(const SEvent &event)
{
	if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN && m_etatJeu != PERDU)
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

			return true;
		}	
	}

	else if(event.EventType == EET_GUI_EVENT)
	{
		if(event.GUIEvent.EventType == EGET_MESSAGEBOX_YES)
		{
			recommencer();
			return true;
		}

		else if(event.GUIEvent.EventType == EGET_MESSAGEBOX_NO)
		{
			m_device->closeDevice();
			return true;
		}

	}

	return false;
}

void Rendu::majSphere()
{
	if(!m_spheresEnVol.empty()) 
	{
		m_clickedSphere = nullptr;
		return;
	}

	if(!m_clickedSphere || m_partie.getTirsRestants() <= 0)
		return;

	m_partie.diminuerTirsRestants();
	m_tirsRestants->setText(stringw(m_partie.getTirsRestants()).c_str());

	s32 i = m_clickedSphere->getID() / m_partie.getTaille();		
	s32 j = m_clickedSphere->getID() % m_partie.getTaille();

	augmenterNiveauSphere(i, j);

	m_clickedSphere = nullptr;
}

void Rendu::augmenterNiveauSphere(int x, int y)
{
	if(!m_sphere[x][y].node)
	{
		m_partie.choixCase(x, y);
		dessinerSphere(x, y);
	}

	else if(m_sphere[x][y].node->getScale() == vector3df(1.0/3))
	{
		m_sphere[x][y].node->setScale(m_sphere[x][y].node->getScale() * 2);
		m_partie.choixCase(x, y);	
	}

	else if(m_sphere[x][y].node->getScale() == vector3df(2.0/3))
	{
		m_sphere[x][y].node->setScale(m_sphere[x][y].node->getScale() * 3.0/2);
		m_partie.choixCase(x, y);
	}

	else if(m_sphere[x][y].node->getScale() == vector3df(1))
		exploserSphere(x, y);
}

void Rendu::exploserSphere(int x, int y)
{
	if(!m_sphere[x][y].node)
		return;

	vector3df destinationMiniSphere, positionMiniSphere;
	f32 tempsAnimation, vitesseAnimation(0.003f);

	for(s32 k = NORD; k <= OUEST; ++k)
	{	
		positionMiniSphere = m_sphere[x][y].node->getPosition();
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
		m_metaSelector->removeTriangleSelector(m_sphere[x][y].node->getTriangleSelector());

		m_miniSphere[k].animatorCollision = m_sceneManager
			->createCollisionResponseAnimator(
					m_metaSelector,
					m_miniSphere[k].node,
					vector3df(1.0/3),
					vector3df(0));

		m_miniSphere[k].animatorCollision->setCollisionCallback(this);

		m_miniSphere[k].animatorDelete = m_sceneManager->createDeleteAnimator(tempsAnimation);

		m_spheresEnVol.push_back(m_miniSphere[k]);
	}

	if(m_sphere[x][y].node)
	{
		m_sceneManager->addToDeletionQueue(m_sphere[x][y].node);
		m_sphere[x][y].node = nullptr;
	}

	m_partie.choixCase(x, y);

	for(auto it = m_miniSphere.begin(); it != m_miniSphere.end(); ++it)
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
	vector3df arrivee(m_sphere[x][y].node->getPosition());

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
	m_device->getGUIEnvironment()->drawAll();
	testAnimator();
	if(m_spheresEnChargement.empty() && m_etapeChargement > 0)
		chargerSpheres();
	m_driver->endScene();
}

void Rendu::lancer()
{
	while(m_device->run())
	{
		if(m_etatJeu != PERDU)
			majSphere();
		rendre();
		rafraichir();
	}
	m_device->drop();
}

void Rendu::rafraichir()
{
	if(m_spheresEnVol.empty() && m_etatJeu != PERDU)
	{
		if(m_partie.resolu())
		{
			clear();
			m_partie.levelUp();
			m_niveau->setText(stringw(m_partie.getNiveauPartie()).c_str());
			m_etapeChargement = 0;
			chargerSpheres();
		}
			majCombos();

		if(m_partie.getTirsRestants() <= 0)
		{
			clear();
			m_etatJeu = PERDU;
			m_device->getGUIEnvironment()->addMessageBox(L"Game Over", L"Vous avez perdu. \nRecommencer ?", true, EMBF_YES|EMBF_NO);
		}

	}

}

void Rendu::majCombos()
{
	if(m_partie.getNbCombos() > 2)
	{
		m_partie.augmenterTirsRestants();
		m_tirsRestants->setText(stringw(m_partie.getTirsRestants()).c_str());
		m_device->getGUIEnvironment()->drawAll();
	}

	m_partie.resetCombos();
}

void Rendu::recommencer()
{
	m_partie.recommencer();
	m_etatJeu = EN_JEU;
	m_etapeChargement = 0;
	chargerSpheres();
}

