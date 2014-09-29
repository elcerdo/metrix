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
            ship_object->setCastShadows(true);
            ship_node = scene->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,0,0));
            ship_node->scale(10,10,10);
            ship_node->attachObject(ship_object);

            ship_node->createChildSceneNode(Ogre::Vector3(0,0,0))->attachObject(player_cam);

            Ogre::Vector3 linear_speed = Ogre::Vector3::ZERO;
            Ogre::Radian rotation_speed(0);
        }

        bool
        frameStarted(const Ogre::FrameEvent& evt)
        {

            mKeyboard->capture();
            mMouse->capture();

            if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
                return false;

            const Ogre::Real& dt = evt.timeSinceLastFrame;

            const Ogre::Vector3 ship_position = ship_node->convertLocalToWorldPosition(Ogre::Vector3::ZERO);
            Ogre::Vector3 ship_unit_x = ship_node->convertLocalToWorldPosition(Ogre::Vector3::UNIT_X) - ship_position;
            Ogre::Vector3 ship_unit_z = ship_node->convertLocalToWorldPosition(Ogre::Vector3::UNIT_Z) - ship_position;
            ship_unit_x.normalise();
            ship_unit_z.normalise();

            if (mKeyboard->isKeyDown(OIS::KC_UP))
                linear_speed += ship_unit_x*1.4;

            if (mKeyboard->isKeyDown(OIS::KC_DOWN))
                linear_speed -= ship_unit_x*1.2;

            if (mKeyboard->isKeyDown(OIS::KC_LEFT))
                rotation_speed += .7;

            if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
                rotation_speed -= .7;

            const Ogre::Real& rotation_tau = .3;
            rotation_speed *= rotation_tau/(rotation_tau+dt);

            const Ogre::Real& translation_ship_x_tau = 2.5;
            const Ogre::Real& translation_ship_z_tau = .5;
            Ogre::Matrix3 foo;
            foo.FromAxes(
                    translation_ship_x_tau/(translation_ship_x_tau+dt)*ship_unit_x,
                    Ogre::Vector3::UNIT_Y,
                    translation_ship_z_tau/(translation_ship_z_tau+dt)*ship_unit_z);
            Ogre::Matrix3 bar;
            bar.FromAxes(
                    ship_unit_x,
                    Ogre::Vector3::UNIT_Y,
                    ship_unit_z);
            linear_speed = foo * bar.Transpose() * linear_speed;

            ship_node->translate(linear_speed*dt, Ogre::SceneNode::TS_PARENT);
            ship_node->yaw(Ogre::Degree(rotation_speed*dt), Ogre::SceneNode::TS_PARENT);

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
        Ogre::Vector3 linear_speed;
        Ogre::Real rotation_speed;

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
        scene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
        scene->setShadowColour(Ogre::ColourValue(1,0,0));
        scene->setAmbientLight(Ogre::ColourValue(0,0,0));

        Ogre::Light* light = scene->createLight("main_light");
        light->setType(Ogre::Light::LT_POINT);
        light->setPosition(Ogre::Vector3(0, 80, 100));
        light->setDiffuseColour(1,1,1);
    }

    {
        Ogre::Camera* mCamera = scene->createCamera("player_cam");

        mCamera->setPosition(Ogre::Vector3(-300,400,0));
        mCamera->lookAt(Ogre::Vector3(200,0,0));
        mCamera->setNearClipDistance(10);

        Ogre::Viewport* vp = window->addViewport(mCamera);
        vp->setBackgroundColour(Ogre::ColourValue(.2,.2,.2));
    }

    {
        Ogre::MeshManager::getSingleton().createPlane(
            "ground_plane.mesh",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3::ZERO),
            2000, 2000,
            20, 20,
            true, 1, 5, 5,
            Ogre::Vector3::UNIT_Z
            );

        Ogre::Entity* ground = scene->createEntity("ground", "ground_plane.mesh");
        ground->setCastShadows(false);
        ground->setMaterialName("ground_mat");

        Ogre::SceneNode* node = scene->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,-30,0));
        node->attachObject(ground);
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

        manual->begin("unicorn_mat", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        manual->position(10,10,0);
        manual->textureCoord(0,0,0);
        manual->position(110,10,0);
        manual->textureCoord(1,0,0);
        manual->position(110,110,10);
        manual->textureCoord(1,1,0);
        manual->position(10,110,0);
        manual->textureCoord(0,1,0);
        manual->normal(0,0,1);
        manual->index(0);
        manual->index(1);
        manual->index(2);
        manual->index(0);
        manual->index(2);
        manual->index(3);
        manual->end();

        manual->setCastShadows(true);

        Ogre::SceneNode* node = scene->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-20,0,0));
        node->attachObject(manual);
    }

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

