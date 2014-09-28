#include <Ogre.h>
#include <OIS.h>
#include <iostream>

using namespace std;

class SimpleFrameListener : public Ogre::FrameListener
{
    public:
        SimpleFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse) :
            Ogre::FrameListener(),
            mKeyboard(keyboard),
            mMouse(mouse)
        {
            Ogre::SceneManager* scene = Ogre::Root::getSingleton().getSceneManager("main_scene");
            player_cam = scene->getCamera("player_cam");
            ship_object = scene->createEntity("ship", "ship.mesh");
            ship_node = scene->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,0,0));
            ship_node->scale(10,10,10);
            ship_node->attachObject(ship_object);
        }

        bool
        frameStarted(const Ogre::FrameEvent& evt)
        {
            mKeyboard->capture();
            mMouse->capture();

            if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
                return false;

            if (mKeyboard->isKeyDown(OIS::KC_UP))
                ship_node->translate(1,0,0, Ogre::SceneNode::TS_LOCAL);

            if (mKeyboard->isKeyDown(OIS::KC_DOWN))
                ship_node->translate(-1,0,0, Ogre::SceneNode::TS_LOCAL);

            if (mKeyboard->isKeyDown(OIS::KC_LEFT))
                ship_node->yaw(Ogre::Degree(1));

            if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
                ship_node->yaw(Ogre::Degree(-1));

            player_cam->lookAt(ship_node->convertLocalToWorldPosition(Ogre::Vector3(1.5,0,0)));
            return true;
        }

        bool
        frameEnded(const Ogre::FrameEvent& evt)
        {
            return true;
        }

    private:
        Ogre::Camera* player_cam;
        Ogre::Entity* ship_object;
        Ogre::SceneNode* ship_node;

        OIS::Keyboard* mKeyboard;
        OIS::Mouse* mMouse;
};

int main(int argc, char* argv[])
{
    Ogre::Root* root = new Ogre::Root();

    if (!root->showConfigDialog()) return -1;

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media", "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    Ogre::RenderWindow* window = root->initialise(true);
    Ogre::SceneManager* scene = root->createSceneManager(Ogre::ST_GENERIC, "main_scene");


    {
        Ogre::Camera* mCamera = scene->createCamera("player_cam");

        mCamera->setPosition(Ogre::Vector3(200,100,500));
        mCamera->lookAt(Ogre::Vector3(0,0,0));
        mCamera->setNearClipDistance(5);

        Ogre::Viewport* vp = window->addViewport(mCamera);
        vp->setBackgroundColour(Ogre::ColourValue(.2,.2,.2));
    }

    {
        Ogre::ManualObject* manual = scene->createManualObject("manual_object");

        manual->begin("unicorn_mat", Ogre::RenderOperation::OT_LINE_STRIP);
        manual->position(0,100,0);
        manual->textureCoord(0,1,0);
        manual->position(0,0,0);
        manual->textureCoord(0,0,0);
        manual->position(100,0,0);
        manual->textureCoord(1,0,0);
        manual->end();

        manual->begin("unicorn_mat", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
        manual->position(10,110,0);
        manual->textureCoord(0,1,0);
        manual->position(10,10,0);
        manual->textureCoord(0,0,0);
        manual->position(110,110,10);
        manual->textureCoord(1,1,0);
        manual->position(110,10,0);
        manual->textureCoord(1,0,0);
        manual->index(0);
        manual->index(1);
        manual->index(3);
        manual->index(0);
        manual->index(1);
        manual->index(2);
        manual->end();

        Ogre::SceneNode* node = scene->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-20,0,0));
        node->attachObject(manual);
    }

    {
        Ogre::MeshManager::getSingleton().createPlane(
            "ground_plane.mesh",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3::ZERO),
            2000, 2000,
            20, 20,
            true, 1, 5, 5,
            Ogre::Vector3::NEGATIVE_UNIT_Z
            );

        Ogre::Entity* ground = scene->createEntity("ground", "ground_plane.mesh");
        ground->setMaterialName("ground_mat");

        Ogre::SceneNode* node = scene->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,-30,0));
        node->attachObject(ground);
    }

    scene->setAmbientLight(Ogre::ColourValue(.9,.9,1.));

    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    window->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    OIS::Mouse* mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
    SimpleFrameListener* frameListener = new SimpleFrameListener(keyboard, mouse);
    root->addFrameListener(frameListener);

    root->startRendering();

    inputManager->destroyInputObject(mouse);
    inputManager->destroyInputObject(keyboard);
    OIS::InputManager::destroyInputSystem(inputManager);

    delete root;

    return 0;
}

