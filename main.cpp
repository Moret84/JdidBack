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

	//cube exemple
	
	scene::IMeshSceneNode* cube = sceneManager->addCubeSceneNode(10.0f,0,-1,core::vector3df(0.0f,0.0f,20.0f));
	cube->setMaterialFlag(video::EMF_WIREFRAME, true);;

//Caméra

sceneManager->addCameraSceneNode();					//Caméra fixe
		
//Rendu

video::SColor blanc(255,255,255,255);

while (device->run())
{
	driver->beginScene(true,true,blanc);
	sceneManager->drawAll();
	driver->endScene();
}

device->drop();
	return 0;
}
