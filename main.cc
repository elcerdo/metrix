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
        }

        bool
        frameStarted(const Ogre::FrameEvent& evt)
        {
            mKeyboard->capture();
            mMouse->capture();

            if(mKeyboard->isKeyDown(OIS::KC_ESCAPE))
                return false;

            return true;
        }

        bool
        frameEnded(const Ogre::FrameEvent& evt)
        {
            return true;
        }

    private:
        OIS::Keyboard* mKeyboard;
        OIS::Mouse* mMouse;
};

int main(int argc, char* argv[])
{
    Ogre::Root* root = new Ogre::Root();

    if (!root->showConfigDialog()) return -1;

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media", "FileSystem");

    Ogre::RenderWindow* window = root->initialise(true);
    Ogre::SceneManager* scene = root->createSceneManager(Ogre::ST_GENERIC);

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    {
        Ogre::Camera* mCamera = scene->createCamera("PlayerCam");

        mCamera->setPosition(Ogre::Vector3(0,0,500));
        mCamera->lookAt(Ogre::Vector3(0,0,0));
        mCamera->setNearClipDistance(5);

        Ogre::Viewport* vp = window->addViewport(mCamera);
        vp->setBackgroundColour(Ogre::ColourValue(.8,0,0));
    }

    {
        Ogre::ManualObject* manual = scene->createManualObject("UnicornMaterial");
        manual->begin("FistSection", Ogre::RenderOperation::OT_LINE_STRIP);
        manual->position(0,100,0);
        manual->position(0,0,0);
        manual->position(100,0,0);
        manual->end();
        scene->getRootSceneNode()->attachObject(manual);
    }

    scene->setAmbientLight(Ogre::ColourValue(.5,.5,1.));

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

