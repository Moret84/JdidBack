#include "Echiquier.hpp"
#include <irrlicht/irrlicht.h>

using namespace irr;

int main()
{

IrrlichtDevice* device = createDevice(				//Création du device
		video::EDT_OPENGL,							//API = OpenGL
		core::dimension2d<u32>(640,480),			//Taille fenêre 640 x 480p
		32);										//32 bits par pixel

video::IVideoDriver* driver = device->getVideoDriver();				//video driver	
scene::ISceneManager * sceneManager = device->getSceneManager();	//scene manager

//Contenu de la scène

f32 x = 0.0f, y = 0.0f, z = 0.0f;
for(int j = 0; j < 4; ++j, x = 0.0f, z += 1.1f)
{
	for(int i = 0; i < 4; ++i, x += 1.1f)
	{
//		scene::IAnimatedMeshSceneNode *caseEchiquier = sceneManager->addAnimatedMeshSceneNode(meshCase, 0, -1, core::vector3df(x,y,z));
		scene::ISceneNode* caseEchiquier = sceneManager->addCubeSceneNode(1.0f, 0, -1, core::vector3df(x,y,z), core::vector3df(0,0,0), core::vector3df(1.0f,0.15f,1.0f));
		caseEchiquier->setMaterialTexture(0, driver->getTexture("caisse.png"));
	}
}

//caseEchiquier->setMaterialFlag(video::EMF_LIGHTING, false);
sceneManager->setAmbientLight(video::SColorf(1.0,1.0,1.0,0.0));
//scene::ILightSceneNode* lumiere = sceneManager->addLightSceneNode(0, core::vector3df(1.4f,3,2.8f));




//Caméra

//sceneManager->addCameraSceneNode(0, core::vector3df(4,5,2), core::vector3df(0,0,0));					//Caméra fixe
sceneManager->addCameraSceneNode(0, core::vector3df(1.4f,3,2.8f), core::vector3df(1.4f,0,1.8f));					//Caméra fixe
//sceneManager->addCameraSceneNodeFPS(0,100.0f,0.1f,-1);
		
//Rendu

video::SColor blanc(255,255,255,255);

while (device->run())
{
	driver->beginScene(true,true, blanc);
	sceneManager->drawAll();
	driver->endScene();
}

device->drop();
	return 0;
}
