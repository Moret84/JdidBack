#include <irrlicht/irrlicht.h>
#include <irrlicht/driverChoice.h>

using namespace irr;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


IrrlichtDevice *Device = 0;
core::stringc StartUpModelFile;
core::stringw MessageText;
core::stringw Caption;
scene::ISceneNode* Model = 0;
scene::ISceneNode* SkyBox = 0;
bool Octree=false;
bool UseLight=false;

scene::ICameraSceneNode* Camera[2] = {0, 0};

// Valeurs utilisées pour identifier les éléments de l'interface utilisateur.
enum
{
    GUI_ID_DIALOG_ROOT_WINDOW  = 0x10000,

    GUI_ID_X_SCALE,
    GUI_ID_Y_SCALE,
    GUI_ID_Z_SCALE,

    GUI_ID_OPEN_MODEL,
    GUI_ID_SET_MODEL_ARCHIVE,
    GUI_ID_LOAD_AS_OCTREE,

    GUI_ID_SKY_BOX_VISIBLE,
    GUI_ID_TOGGLE_DEBUG_INFO,

    GUI_ID_DEBUG_OFF,
    GUI_ID_DEBUG_BOUNDING_BOX,
    GUI_ID_DEBUG_NORMALS,
    GUI_ID_DEBUG_SKELETON,
    GUI_ID_DEBUG_WIRE_OVERLAY,
    GUI_ID_DEBUG_HALF_TRANSPARENT,
    GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES,
    GUI_ID_DEBUG_ALL,

    GUI_ID_MODEL_MATERIAL_SOLID,
    GUI_ID_MODEL_MATERIAL_TRANSPARENT,
    GUI_ID_MODEL_MATERIAL_REFLECTION,

    GUI_ID_CAMERA_MAYA,
    GUI_ID_CAMERA_FIRST_PERSON,

    GUI_ID_POSITION_TEXT,

    GUI_ID_ABOUT,
    GUI_ID_QUIT,

    GUI_ID_TEXTUREFILTER,
    GUI_ID_SKIN_TRANSPARENCY,
    GUI_ID_SKIN_ANIMATION_FPS,

    GUI_ID_BUTTON_SET_SCALE,
    GUI_ID_BUTTON_SCALE_MUL10,
    GUI_ID_BUTTON_SCALE_DIV10,
    GUI_ID_BUTTON_OPEN_MODEL,
    GUI_ID_BUTTON_SHOW_ABOUT,
    GUI_ID_BUTTON_SHOW_TOOLBOX,
    GUI_ID_BUTTON_SELECT_ARCHIVE,

    GUI_ID_ANIMATION_INFO,

    // Et quelques nombres magiques.
    MAX_FRAMERATE = 80,
    DEFAULT_FRAMERATE = 30
};


void setActiveCamera(scene::ICameraSceneNode* newActive)
{
    if (0 == Device)
        return;

    scene::ICameraSceneNode * active = Device->getSceneManager()->getActiveCamera();
    active->setInputReceiverEnabled(false);

    newActive->setInputReceiverEnabled(true);
    Device->getSceneManager()->setActiveCamera(newActive);
}

void setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin)
{
    for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
        video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
        col.setAlpha(alpha);
        skin->setColor((EGUI_DEFAULT_COLOR)i, col);
    }
}

void updateScaleInfo(scene::ISceneNode* model)
{
    IGUIElement* toolboxWnd = Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
    if (!toolboxWnd)
        return;
    if (!model)
    {
        toolboxWnd->getElementFromId(GUI_ID_X_SCALE, true)->setText( L"-" );
        toolboxWnd->getElementFromId(GUI_ID_Y_SCALE, true)->setText( L"-" );
        toolboxWnd->getElementFromId(GUI_ID_Z_SCALE, true)->setText( L"-" );
    }
    else
    {
        core::vector3df scale = model->getScale();
        toolboxWnd->getElementFromId(GUI_ID_X_SCALE, true)->setText( core::stringw(scale.X).c_str() );
        toolboxWnd->getElementFromId(GUI_ID_Y_SCALE, true)->setText( core::stringw(scale.Y).c_str() );
        toolboxWnd->getElementFromId(GUI_ID_Z_SCALE, true)->setText( core::stringw(scale.Z).c_str() );
    }
}

void showAboutText()
{
    // Crée une fenêtre modale avec le texte
    // chargé à partir du fichier XML.
    Device->getGUIEnvironment()->addMessageBox(
        Caption.c_str(), MessageText.c_str());
}

void loadModel(const c8* fn)
{
    // Modifie le nom si c'est un fichier .pk3.

    io::path filename(fn);

    io::path extension;
    core::getFileNameExtension(extension, filename);
    extension.make_lower();

    // Si une texture est chargée, l'applique au modèle courant… 
    if (extension == ".jpg" || extension == ".pcx" ||
        extension == ".png" || extension == ".ppm" ||
        extension == ".pgm" || extension == ".pbm" ||
        extension == ".psd" || extension == ".tga" ||
        extension == ".bmp" || extension == ".wal" ||
        extension == ".rgb" || extension == ".rgba")
    {
        video::ITexture * texture =
            Device->getVideoDriver()->getTexture( filename );
        if ( texture && Model )
        {
            // Recharge toujours la texture.
            Device->getVideoDriver()->removeTexture(texture);
            texture = Device->getVideoDriver()->getTexture( filename );

            Model->setMaterialTexture(0, texture);
        }
        return;
    }
    // Si une archive est chargée, l'ajoute à FileArchive.
    else if (extension == ".pk3" || extension == ".zip" || extension == ".pak" || extension == ".npk")
    {
        Device->getFileSystem()->addFileArchive(filename.c_str());
        return;
    }

    // Charge le modèle dans le moteur.

    if (Model)
        Model->remove();

    Model = 0;

    if (extension==".irr")
    {
        core::array<scene::ISceneNode*> outNodes;
        Device->getSceneManager()->loadScene(filename);
        Device->getSceneManager()->getSceneNodesFromType(scene::ESNT_ANIMATED_MESH, outNodes);
        if (outNodes.size())
            Model = outNodes[0];
        return;
    }

    scene::IAnimatedMesh* m = Device->getSceneManager()->getMesh( filename.c_str() );

    if (!m)
    {
        // Le modèle ne peut pas être chargé.

        if (StartUpModelFile != filename)
            Device->getGUIEnvironment()->addMessageBox(
            Caption.c_str(), L"The model could not be loaded. " \
            L"Maybe it is not a supported file format.");
        return;
    }

    // Met les propriétés du matériel par défaut.

    if (Octree)
        Model = Device->getSceneManager()->addOctreeSceneNode(m->getMesh(0));
    else
    {
        scene::IAnimatedMeshSceneNode* animModel = Device->getSceneManager()->addAnimatedMeshSceneNode(m);
        animModel->setAnimationSpeed(30);
        Model = animModel;
    }
    Model->setMaterialFlag(video::EMF_LIGHTING, UseLight);
    Model->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, UseLight);
//  Model->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    Model->setDebugDataVisible(scene::EDS_OFF);

    // Nous avons besoin de décocher les entrées du menuw. Ce serait bien de simuler un événement menu mais
    // ce n'est pas simple. Donc nous le faisons à la manière brute.
    gui::IGUIContextMenu* menu = (gui::IGUIContextMenu*)Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_TOGGLE_DEBUG_INFO, true);
    if (menu)
        for(int item = 1; item < 6; ++item)
            menu->setItemChecked(item, false);
    updateScaleInfo(Model);
}

void createToolBox()
{
    // Supprime la boîte d'outils s'il y en déjà une.
    IGUIEnvironment* env = Device->getGUIEnvironment();
    IGUIElement* root = env->getRootGUIElement();
    IGUIElement* e = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
    if (e)
        e->remove();

    // Crée la boîte d'outils de la fenêtre.
    IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,45,800,480),
        false, L"Toolset", 0, GUI_ID_DIALOG_ROOT_WINDOW);

    // Crée le système d'onglets et les onglets.
    IGUITabControl* tab = env->addTabControl(
        core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

    IGUITab* t1 = tab->addTab(L"Config");

    // Ajoute quelques zones de texte ainsi qu'un bouton sur le premier onglet.
    env->addStaticText(L"Scale:",
            core::rect<s32>(10,20,60,45), false, false, t1);
    env->addStaticText(L"X:", core::rect<s32>(22,48,40,66), false, false, t1);
    env->addEditBox(L"1.0", core::rect<s32>(40,46,130,66), true, t1, GUI_ID_X_SCALE);
    env->addStaticText(L"Y:", core::rect<s32>(22,82,40,96), false, false, t1);
    env->addEditBox(L"1.0", core::rect<s32>(40,76,130,96), true, t1, GUI_ID_Y_SCALE);
    env->addStaticText(L"Z:", core::rect<s32>(22,108,40,126), false, false, t1);
    env->addEditBox(L"1.0", core::rect<s32>(40,106,130,126), true, t1, GUI_ID_Z_SCALE);

    env->addButton(core::rect<s32>(10,134,85,165), t1, GUI_ID_BUTTON_SET_SCALE, L"Set");

    // Boutons de redimensionnement rapide.
    env->addButton(core::rect<s32>(65,20,95,40), t1, GUI_ID_BUTTON_SCALE_MUL10, L"* 10");
    env->addButton(core::rect<s32>(100,20,130,40), t1, GUI_ID_BUTTON_SCALE_DIV10, L"* 0.1");

    updateScaleInfo(Model);

    // Ajoute le contrôle de la transparence.
    env->addStaticText(L"GUI Transparency Control:",
            core::rect<s32>(10,200,150,225), true, false, t1);
    IGUIScrollBar* scrollbar = env->addScrollBar(true,
            core::rect<s32>(10,225,150,240), t1, GUI_ID_SKIN_TRANSPARENCY);
    scrollbar->setMax(255);
    scrollbar->setPos(255);

    // Ajoute le contrôle du nombre d'images par seconde.
    env->addStaticText(L":", core::rect<s32>(10,240,150,265), true, false, t1);
    env->addStaticText(L"Framerate:",
            core::rect<s32>(12,240,75,265), false, false, t1);
    // Informations sur l'image courante.
    env->addStaticText(L"", core::rect<s32>(75,240,200,265), false, false, t1,
            GUI_ID_ANIMATION_INFO);
    scrollbar = env->addScrollBar(true,
            core::rect<s32>(10,265,150,280), t1, GUI_ID_SKIN_ANIMATION_FPS);
    scrollbar->setMax(MAX_FRAMERATE);
    scrollbar->setMin(-MAX_FRAMERATE);
    scrollbar->setPos(DEFAULT_FRAMERATE);
    scrollbar->setSmallStep(1);
}

void updateToolBox()
{
    IGUIEnvironment* env = Device->getGUIEnvironment();
    IGUIElement* root = env->getRootGUIElement();
    IGUIElement* dlg = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
    if (!dlg )
        return;

    // Met à jour les informations que nous avons sur l'animation du modèle.
    IGUIStaticText *  aniInfo = (IGUIStaticText *)(dlg->getElementFromId(GUI_ID_ANIMATION_INFO, true));
    if (aniInfo)
    {
        if ( Model && scene::ESNT_ANIMATED_MESH == Model->getType() )
        {
            scene::IAnimatedMeshSceneNode* animatedModel = (scene::IAnimatedMeshSceneNode*)Model;

            core::stringw str( (s32)core::round_(animatedModel->getAnimationSpeed()) );
            str += L" Frame: ";
            str += core::stringw((s32)animatedModel->getFrameNr());
            aniInfo->setText(str.c_str());
        }
        else
            aniInfo->setText(L"");
    }
}

void onKillFocus()
{
    // Évite que la caméra FPS continue de bouger quand l'utilisateur presse le bouton alt-tab pendant le déplacement de la caméra.
    const core::list<scene::ISceneNodeAnimator*>& animators = Camera[1]->getAnimators();
    core::list<irr::scene::ISceneNodeAnimator*>::ConstIterator iter = animators.begin();
    while ( iter != animators.end() )
    {
        if ( (*iter)->getType() == scene::ESNAT_CAMERA_FPS )
        {
            // Nous envoyons un événement de pression de touche de clavier pour chaque touche utilisée dans cet animateur.
            scene::ISceneNodeAnimatorCameraFPS * fpsAnimator = static_cast<scene::ISceneNodeAnimatorCameraFPS*>(*iter);
            const core::array<SKeyMap>& keyMap = fpsAnimator->getKeyMap();
            for ( irr::u32 i=0; i< keyMap.size(); ++i )
            {
                irr::SEvent event;
                event.EventType = EET_KEY_INPUT_EVENT;
                event.KeyInput.Key = keyMap[i].KeyCode;
                event.KeyInput.PressedDown = false;
                fpsAnimator->OnEvent(event);
            }
        }
        ++iter;
    }
}

bool hasModalDialog()
{
    if ( !Device )
        return false;
    IGUIEnvironment* env = Device->getGUIEnvironment();
    IGUIElement * focused = env->getFocus();
    while ( focused )
    {
        if ( focused->isVisible() && focused->hasType(EGUIET_MODAL_SCREEN) )
            return true;
        focused = focused->getParent();
    }
    return false;
}


class MyEventReceiver : public IEventReceiver
{
public:
    virtual bool OnEvent(const SEvent& event)
    {
        // La touche Echap change la caméra utilisée.
        if (event.EventType == EET_KEY_INPUT_EVENT &&
            event.KeyInput.PressedDown == false)
        {
            if ( OnKeyUp(event.KeyInput.Key) )
                return true;
        }

        if (event.EventType == EET_GUI_EVENT)
        {
            s32 id = event.GUIEvent.Caller->getID();
            IGUIEnvironment* env = Device->getGUIEnvironment();

            switch(event.GUIEvent.EventType)
            {
            case EGET_MENU_ITEM_SELECTED:
                    // Un élément de menu a été cliqué.
                    OnMenuItemSelected( (IGUIContextMenu*)event.GUIEvent.Caller );
                break;

            case EGET_FILE_SELECTED:
                {
                    // Charge le modèle sélectionné dans la fenêtre d'ouverture de fichiers.
                    IGUIFileOpenDialog* dialog =
                        (IGUIFileOpenDialog*)event.GUIEvent.Caller;
                    loadModel(core::stringc(dialog->getFileName()).c_str());
                }
                break;

            case EGET_SCROLL_BAR_CHANGED:

                // Contrôle de la transparence du « skin ».
                if (id == GUI_ID_SKIN_TRANSPARENCY)
                {
                    const s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
                    setSkinTransparency(pos, env->getSkin());
                }
                // Contrôle la vitesse d'animation.
                else if (id == GUI_ID_SKIN_ANIMATION_FPS)
                {
                    const s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
                    if (scene::ESNT_ANIMATED_MESH == Model->getType())
                        ((scene::IAnimatedMeshSceneNode*)Model)->setAnimationSpeed((f32)pos);
                }
                break;

            case EGET_COMBO_BOX_CHANGED:

                // Contrôle l'anti-aliasing.
                if (id == GUI_ID_TEXTUREFILTER)
                {
                    OnTextureFilterSelected( (IGUIComboBox*)event.GUIEvent.Caller );
                }
                break;

            case EGET_BUTTON_CLICKED:

                switch(id)
                {
                case GUI_ID_BUTTON_SET_SCALE:
                    {
                        // Fixe l'échelle.
                        gui::IGUIElement* root = env->getRootGUIElement();
                        core::vector3df scale;
                        core::stringc s;

                        s = root->getElementFromId(GUI_ID_X_SCALE, true)->getText();
                        scale.X = (f32)atof(s.c_str());
                        s = root->getElementFromId(GUI_ID_Y_SCALE, true)->getText();
                        scale.Y = (f32)atof(s.c_str());
                        s = root->getElementFromId(GUI_ID_Z_SCALE, true)->getText();
                        scale.Z = (f32)atof(s.c_str());

                        if (Model)
                            Model->setScale(scale);
                        updateScaleInfo(Model);
                    }
                    break;
                case GUI_ID_BUTTON_SCALE_MUL10:
                    if (Model)
                        Model->setScale(Model->getScale()*10.f);
                    updateScaleInfo(Model);
                    break;
                case GUI_ID_BUTTON_SCALE_DIV10:
                    if (Model)
                        Model->setScale(Model->getScale()*0.1f);
                    updateScaleInfo(Model);
                    break;
                case GUI_ID_BUTTON_OPEN_MODEL:
                    env->addFileOpenDialog(L"Please select a model file to open");
                    break;
                case GUI_ID_BUTTON_SHOW_ABOUT:
                    showAboutText();
                    break;
                case GUI_ID_BUTTON_SHOW_TOOLBOX:
                    createToolBox();
                    break;
                case GUI_ID_BUTTON_SELECT_ARCHIVE:
                    env->addFileOpenDialog(L"Please select your game archive/directory");
                    break;
                }

                break;
            default:
                break;
            }
        }

        return false;
    }

    
        bool OnKeyUp(irr::EKEY_CODE keyCode)
    {
        // Ne gère pas les touches si nous avons une fenêtre modale ouverte ce qui provoquerait un comportement inattendu pour utilisateur.
        if ( hasModalDialog() )
            return false;
        
        if (keyCode == irr::KEY_ESCAPE)
        {
            if (Device)
            {
                scene::ICameraSceneNode * camera =
                    Device->getSceneManager()->getActiveCamera();
                if (camera)
                {
                    camera->setInputReceiverEnabled( !camera->isInputReceiverEnabled() );
                }
                return true;
            }
        }
        else if (keyCode == irr::KEY_F1)
        {
            if (Device)
            {
                IGUIElement* elem = Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_POSITION_TEXT);
                if (elem)
                    elem->setVisible(!elem->isVisible());
            }
        }
        else if (keyCode == irr::KEY_KEY_M)
        {
            if (Device)
                Device->minimizeWindow();
        }
        else if (keyCode == irr::KEY_KEY_L)
        {
            UseLight=!UseLight;
            if (Model)
            {
                Model->setMaterialFlag(video::EMF_LIGHTING, UseLight);
                Model->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, UseLight);
            }
        }
        return false;
    }

        void OnMenuItemSelected( IGUIContextMenu* menu )
    {
        s32 id = menu->getItemCommandId(menu->getSelectedItem());
        IGUIEnvironment* env = Device->getGUIEnvironment();

        switch(id)
        {
        case GUI_ID_OPEN_MODEL: // FilOnButtonSetScalinge -> Open Model
            env->addFileOpenDialog(L"Please select a model file to open");
            break;
        case GUI_ID_SET_MODEL_ARCHIVE: // File -> Set Model Archive
            env->addFileOpenDialog(L"Please select your game archive/directory");
            break;
        case GUI_ID_LOAD_AS_OCTREE: // File -> LoadAsOctree
            Octree = !Octree;
            menu->setItemChecked(menu->getSelectedItem(), Octree);
            break;
        case GUI_ID_QUIT: // File -> Quit
            Device->closeDevice();
            break;
        case GUI_ID_SKY_BOX_VISIBLE: // View -> Skybox
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            SkyBox->setVisible(!SkyBox->isVisible());
            break;
        case GUI_ID_DEBUG_OFF: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem()+1, false);
            menu->setItemChecked(menu->getSelectedItem()+2, false);
            menu->setItemChecked(menu->getSelectedItem()+3, false);
            menu->setItemChecked(menu->getSelectedItem()+4, false);
            menu->setItemChecked(menu->getSelectedItem()+5, false);
            menu->setItemChecked(menu->getSelectedItem()+6, false);
            if (Model)
                Model->setDebugDataVisible(scene::EDS_OFF);
            break;
        case GUI_ID_DEBUG_BOUNDING_BOX: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            if (Model)
                Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_BBOX));
            break;
        case GUI_ID_DEBUG_NORMALS: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            if (Model)
                Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_NORMALS));
            break;
        case GUI_ID_DEBUG_SKELETON: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            if (Model)
                Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_SKELETON));
            break;
        case GUI_ID_DEBUG_WIRE_OVERLAY: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            if (Model)
                Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_MESH_WIRE_OVERLAY));
            break;
        case GUI_ID_DEBUG_HALF_TRANSPARENT: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            if (Model)
                Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_HALF_TRANSPARENCY));
            break;
        case GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
            if (Model)
                Model->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(Model->isDebugDataVisible()^scene::EDS_BBOX_BUFFERS));
            break;
        case GUI_ID_DEBUG_ALL: // View -> Debug Information
            menu->setItemChecked(menu->getSelectedItem()-1, true);
            menu->setItemChecked(menu->getSelectedItem()-2, true);
            menu->setItemChecked(menu->getSelectedItem()-3, true);
            menu->setItemChecked(menu->getSelectedItem()-4, true);
            menu->setItemChecked(menu->getSelectedItem()-5, true);
            menu->setItemChecked(menu->getSelectedItem()-6, true);
            if (Model)
                Model->setDebugDataVisible(scene::EDS_FULL);
            break;
        case GUI_ID_ABOUT: // Help->About
            showAboutText();
            break;
        case GUI_ID_MODEL_MATERIAL_SOLID: // View -> Material -> Solid
            if (Model)
                Model->setMaterialType(video::EMT_SOLID);
            break;
        case GUI_ID_MODEL_MATERIAL_TRANSPARENT: // View -> Material -> Transparent
            if (Model)
                Model->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
            break;
        case GUI_ID_MODEL_MATERIAL_REFLECTION: // View -> Material -> Reflection
            if (Model)
                Model->setMaterialType(video::EMT_SPHERE_MAP);
            break;

        case GUI_ID_CAMERA_MAYA:
            setActiveCamera(Camera[0]);
            break;
        case GUI_ID_CAMERA_FIRST_PERSON:
            setActiveCamera(Camera[1]);
            break;
        }
    }

        void OnTextureFilterSelected( IGUIComboBox* combo )
    {
        s32 pos = combo->getSelected();
        switch (pos)
        {
            case 0:
            if (Model)
            {
                Model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
                Model->setMaterialFlag(video::EMF_TRILINEAR_FILTER, false);
                Model->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, false);
            }
            break;
            case 1:
            if (Model)
            {
                Model->setMaterialFlag(video::EMF_BILINEAR_FILTER, true);
                Model->setMaterialFlag(video::EMF_TRILINEAR_FILTER, false);
            }
            break;
            case 2:
            if (Model)
            {
                Model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
                Model->setMaterialFlag(video::EMF_TRILINEAR_FILTER, true);
            }
            break;
            case 3:
            if (Model)
            {
                Model->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, true);
            }
            break;
            case 4:
            if (Model)
            {
                Model->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, false);
            }
            break;
        }
    }
};

int main(int argc, char* argv[])
{
    // Demande un pilote à l'utilisateur.
    video::E_DRIVER_TYPE driverType=driverChoiceConsole();
    if (driverType==video::EDT_COUNT)
        return 1;

    // Crée le moteur qui quitte si la création a échoué.
    MyEventReceiver receiver;
    Device = createDevice(driverType, core::dimension2d<u32>(800, 600),
        16, false, false, false, &receiver);

    if (Device == 0)
        return 1; // Ne peut pas créer le pilote sélectionné.

    Device->setResizable(true);

    Device->setWindowCaption(L"Irrlicht Engine - Loading...");

    video::IVideoDriver* driver = Device->getVideoDriver();
    IGUIEnvironment* env = Device->getGUIEnvironment();
    scene::ISceneManager* smgr = Device->getSceneManager();
    smgr->getParameters()->setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);

    driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

    smgr->addLightSceneNode(0, core::vector3df(200,200,200),
        video::SColorf(1.0f,1.0f,1.0f),2000);
    smgr->setAmbientLight(video::SColorf(0.3f,0.3f,0.3f));
    // Ajoute notre dossier « media » comme « répertoire de recherche ».
    Device->getFileSystem()->addFileArchive("media/");

        // Lit la configuration à partir du fichier XML.

    io::IXMLReader* xml = Device->getFileSystem()->createXMLReader( L"config.xml");

    while(xml && xml->read())
    {
        switch(xml->getNodeType())
        {
        case io::EXN_TEXT:
            // Dans le fichier XML, le seul texte que nous ayons est le messageText.
            MessageText = xml->getNodeData();
            break;
        case io::EXN_ELEMENT:
            {
                if (core::stringw("startUpModel") == xml->getNodeName())
                    StartUpModelFile = xml->getAttributeValue(L"file");
                else
                if (core::stringw("messageText") == xml->getNodeName())
                    Caption = xml->getAttributeValue(L"caption");
            }
            break;
        default:
            break;
        }
    }

    if (xml)
        xml->drop(); // Ne pas oublier de supprimer le lecteur XML.

    if (argc > 1)
        StartUpModelFile = argv[1];

        // Met une jolie police.

    IGUISkin* skin = env->getSkin();
    IGUIFont* font = env->getFont("fonthaettenschweiler.bmp");
    if (font)
        skin->setFont(font);

    // Crée le menu.
    gui::IGUIContextMenu* menu = env->addMenu();
    menu->addItem(L"File", -1, true, true);
    menu->addItem(L"View", -1, true, true);
    menu->addItem(L"Camera", -1, true, true);
    menu->addItem(L"Help", -1, true, true);

    gui::IGUIContextMenu* submenu;
    submenu = menu->getSubMenu(0);
    submenu->addItem(L"Open Model File & Texture...", GUI_ID_OPEN_MODEL);
    submenu->addItem(L"Set Model Archive...", GUI_ID_SET_MODEL_ARCHIVE);
    submenu->addItem(L"Load as Octree", GUI_ID_LOAD_AS_OCTREE);
    submenu->addSeparator();
    submenu->addItem(L"Quit", GUI_ID_QUIT);

    submenu = menu->getSubMenu(1);
    submenu->addItem(L"sky box visible", GUI_ID_SKY_BOX_VISIBLE, true, false, true);
    submenu->addItem(L"toggle model debug information", GUI_ID_TOGGLE_DEBUG_INFO, true, true);
    submenu->addItem(L"model material", -1, true, true );

    submenu = submenu->getSubMenu(1);
    submenu->addItem(L"Off", GUI_ID_DEBUG_OFF);
    submenu->addItem(L"Bounding Box", GUI_ID_DEBUG_BOUNDING_BOX);
    submenu->addItem(L"Normals", GUI_ID_DEBUG_NORMALS);
    submenu->addItem(L"Skeleton", GUI_ID_DEBUG_SKELETON);
    submenu->addItem(L"Wire overlay", GUI_ID_DEBUG_WIRE_OVERLAY);
    submenu->addItem(L"Half-Transparent", GUI_ID_DEBUG_HALF_TRANSPARENT);
    submenu->addItem(L"Buffers bounding boxes", GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES);
    submenu->addItem(L"All", GUI_ID_DEBUG_ALL);

    submenu = menu->getSubMenu(1)->getSubMenu(2);
    submenu->addItem(L"Solid", GUI_ID_MODEL_MATERIAL_SOLID);
    submenu->addItem(L"Transparent", GUI_ID_MODEL_MATERIAL_TRANSPARENT);
    submenu->addItem(L"Reflection", GUI_ID_MODEL_MATERIAL_REFLECTION);

    submenu = menu->getSubMenu(2);
    submenu->addItem(L"Maya Style", GUI_ID_CAMERA_MAYA);
    submenu->addItem(L"First Person", GUI_ID_CAMERA_FIRST_PERSON);

    submenu = menu->getSubMenu(3);
    submenu->addItem(L"About", GUI_ID_ABOUT);

        // Crée la barre d'outils.

    gui::IGUIToolBar* bar = env->addToolBar();

    video::ITexture* image = driver->getTexture("open.png");
    bar->addButton(GUI_ID_BUTTON_OPEN_MODEL, 0, L"Open a model",image, 0, false, true);

    image = driver->getTexture("tools.png");
    bar->addButton(GUI_ID_BUTTON_SHOW_TOOLBOX, 0, L"Open Toolset",image, 0, false, true);

    image = driver->getTexture("zip.png");
    bar->addButton(GUI_ID_BUTTON_SELECT_ARCHIVE, 0, L"Set Model Archive",image, 0, false, true);

    image = driver->getTexture("help.png");
    bar->addButton(GUI_ID_BUTTON_SHOW_ABOUT, 0, L"Open Help", image, 0, false, true);

    // Crée la liste déroulante pour les filtres de textures.

    gui::IGUIComboBox* box = env->addComboBox(core::rect<s32>(250,4,350,23), bar, GUI_ID_TEXTUREFILTER);
    box->addItem(L"No filtering");
    box->addItem(L"Bilinear");
    box->addItem(L"Trilinear");
    box->addItem(L"Anisotropic");
    box->addItem(L"Isotropic");

        // Désactive l'alpha.

    for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
    {
        video::SColor col = env->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        env->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
    }

    // Ajoute un système d'onglets.

    createToolBox();

    // Créé le texte pour le nombre d'images par seconde.

    IGUIStaticText* fpstext = env->addStaticText(L"",
            core::rect<s32>(400,4,570,23), true, false, bar);

    IGUIStaticText* postext = env->addStaticText(L"",
            core::rect<s32>(10,50,470,80),false, false, 0, GUI_ID_POSITION_TEXT);
    postext->setVisible(false);

    // Met le titre de la fenêtre.

    Caption += " - [";
    Caption += driver->getName();
    Caption += "]";
    Device->setWindowCaption(Caption.c_str());

        // Montre une pop-up « à propos » et charge le modèle par défaut.
    if (argc==1)
        showAboutText();
    loadModel(StartUpModelFile.c_str());

    // Ajoute la « skybox ».

    SkyBox = smgr->addSkyBoxSceneNode(
        driver->getTexture("irrlicht2_up.jpg"),
        driver->getTexture("irrlicht2_dn.jpg"),
        driver->getTexture("irrlicht2_lf.jpg"),
        driver->getTexture("irrlicht2_rt.jpg"),
        driver->getTexture("irrlicht2_ft.jpg"),
        driver->getTexture("irrlicht2_bk.jpg"));

    // Ajoute le nœud de scène de caméra.
    Camera[0] = smgr->addCameraSceneNodeMaya();
    Camera[0]->setFarValue(20000.f);
    // Les caméras Maya se repositionnent elles-mêmes relativement à leur cible, donc à la position de la cible où le nœud de scène de mesh est placé.
    Camera[0]->setTarget(core::vector3df(0,30,0));

    Camera[1] = smgr->addCameraSceneNodeFPS();
    Camera[1]->setFarValue(20000.f);
    Camera[1]->setPosition(core::vector3df(0,0,-70));
    Camera[1]->setTarget(core::vector3df(0,30,0));

    setActiveCamera(Camera[0]);

    // Charge le logo du moteur Irrlicht.
    IGUIImage *img =
        env->addImage(driver->getTexture("irrlichtlogo2.png"),
            core::position2d<s32>(10, driver->getScreenSize().Height - 128));

    // Fixe les bords du logo dans le coin en bas à gauche de l'écran.
    img->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,
            EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

    // Se rappelle de l'état donc nous pourrons remarquer quand le fenêtre perd le focus.
    bool hasFocus = Device->isWindowFocused();

    // Dessine tout.

    while(Device->run() && driver)
    {
        // Capte les changements de focus (contournement en attendant qu'Irllicht intègre des événements pour cela).
        bool focused = Device->isWindowFocused();
        if ( hasFocus && !focused )
            onKillFocus();
        hasFocus = focused;

        if (Device->isWindowActive())
        {
            driver->beginScene(true, true, video::SColor(150,50,50,50));

            smgr->drawAll();
            env->drawAll();

            driver->endScene();

            // Met à jour les informations sur le nombre d'images par seconde.
            core::stringw str(L"FPS: ");
            str.append(core::stringw(driver->getFPS()));
            str += L" Tris: ";
            str.append(core::stringw(driver->getPrimitiveCountDrawn()));
            fpstext->setText(str.c_str());

            // Met à jour les informations sur la caméra courante.
            scene::ICameraSceneNode* cam = Device->getSceneManager()->getActiveCamera();
            str = L"Pos: ";
            str.append(core::stringw(cam->getPosition().X));
            str += L" ";
            str.append(core::stringw(cam->getPosition().Y));
            str += L" ";
            str.append(core::stringw(cam->getPosition().Z));
            str += L" Tgt: ";
            str.append(core::stringw(cam->getTarget().X));
            str += L" ";
            str.append(core::stringw(cam->getTarget().Y));
            str += L" ";
            str.append(core::stringw(cam->getTarget().Z));
            postext->setText(str.c_str());

            // Met à jour la fenêtre d'outils.
            updateToolBox();
        }
        else
            Device->yield();
    }

    Device->drop();
    return 0;
}
