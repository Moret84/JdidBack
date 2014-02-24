
using namespace irr;

IrrlichtDevice* device = createDevice(				//Création du device
		video::EDT_OPENGL,							//API = OpenGL
		core::dimension2D<u32>(200,200),			//Taille fenêre 640 x 480p
		32);										//32 bits par pixel

video::IVideoDrievr* driver = device->getVideoDriver();				//video driver	

scene::ISceneManager * sceneManager = device->getSceneManager();	//scene manager

//Caméra

sceneManager->addCameraSceneNode();					//Caméra fixe
		
//Rendu

while (device->run())
{
	driver->beginScene(true,true);
	sceneManager->drawAll();
	driver->endScene();
}

device->drop();

		
