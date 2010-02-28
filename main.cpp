#include <Ogre.h>
using namespace Ogre;

#include <OIS/OIS.h>

#include <deque>
//using namespace std;

class ExitListener : public FrameListener
{
public:
    ExitListener(OIS::Keyboard *keyboard)
        : mKeyboard(keyboard)
    {
    }

    bool frameStarted(const FrameEvent& evt)
    {
        mKeyboard->capture();
        return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
    }

private:
    OIS::Keyboard *mKeyboard;
};

class Application;

class ApplicationListener : public FrameListener, public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
    ApplicationListener(Root *root, OIS::InputManager *inputManager, Application *app, Camera *camera, SceneManager *sceneMgr, SceneNode *sn,
        Entity *ent, std::deque<Vector3> &walk)
        : mRoot(root), mInputManager(inputManager), mApp(app), mCamera(camera), mSceneMgr(sceneMgr), mNode(sn), mEntity(ent), mWalkList(walk)
    {
        /*try
        {*/
            mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
            mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
            //mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, false));
        /*}
        catch (const OIS::Exception &e)
        {
            throw new Exception(42, e.eText, "Application::setupInputSystem");
        }*/

        mKeyboard->setEventCallback(this);
        mMouse->setEventCallback(this);
        //mJoy->setEventCallback(this);

        // Populate the camera container
        mCamNode = mCamera->getParentSceneNode();

        // set the rotation and move speed
        mRotate = 0.13;
        mMove = 250;

        // continue rendering
        mContinue = true;

        // not moving at first
        mCharDirection = Vector3::ZERO;
        mCowDirection = Vector3::ZERO;

        // Set idle animation
        mAnimationState = ent->getAnimationState("Walk");
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);
    }

    ~ApplicationListener()
    {
        mInputManager->destroyInputObject(mKeyboard);
        mInputManager->destroyInputObject(mMouse);
        //mInputManager->destroyInputObject(mJoy);
    }

    // This function is called to start the object moving to the next position
    // in mWalkList.
    bool nextLocation()
    {
        return true;
    }

    // FrameListener
    bool frameStarted(const FrameEvent& evt)
    {
        mKeyboard->capture();
        mMouse->capture();
        //mJoy->capture();

        mCamNode->translate(mCharDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);
        mNode->translate(mCowDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);

        mAnimationState->addTime(evt.timeSinceLastFrame);

        return mContinue;
    }

    // KeyListener
    bool keyPressed(const OIS::KeyEvent &arg)
    {
        switch (arg.key)
        {
        case OIS::KC_ESCAPE:
            mContinue = false;
            break;


        case OIS::KC_UP:
            mCowDirection.z = mMove;
            break;

        case OIS::KC_DOWN:
            mCowDirection.z = -mMove;
            break;

        case OIS::KC_LEFT:
            mCowDirection.x = mMove;
            break;

        case OIS::KC_RIGHT:
            mCowDirection.x = -mMove;
            break;

        case OIS::KC_PGDOWN:
            mCowDirection.y = -mMove;
            break;

        case OIS::KC_PGUP:
            mCowDirection.y = mMove;
            break;


        case OIS::KC_W:
            mCharDirection.z = -mMove;
            break;

        case OIS::KC_S:
            mCharDirection.z = mMove;
            break;

        case OIS::KC_A:
            mCharDirection.x = -mMove;
            break;

        case OIS::KC_D:
            mCharDirection.x = mMove;
            break;

        case OIS::KC_E:
            mCharDirection.y = -mMove;
            break;

        case OIS::KC_Q:
            mCharDirection.y = mMove;
            break;

        default:
            break;
        }
        return mContinue;
    }

    bool keyReleased(const OIS::KeyEvent &arg)
    {
        switch (arg.key)
        {
        case OIS::KC_UP:
        case OIS::KC_DOWN:
            mCowDirection.z = 0;
            break;

        case OIS::KC_LEFT:
        case OIS::KC_RIGHT:
            mCowDirection.x = 0;
            break;

        case OIS::KC_PGDOWN:
        case OIS::KC_PGUP:
            mCowDirection.y = 0;
            break;


        case OIS::KC_W:
        case OIS::KC_S:
            mCharDirection.z = 0;
            break;

        case OIS::KC_A:
        case OIS::KC_D:
            mCharDirection.x = 0;
            break;

        case OIS::KC_E:
        case OIS::KC_Q:
            mCharDirection.y = 0;
            break;

        default:
            break;
        } // switch
        return true;
    }

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &arg)
    {
        if (arg.state.buttonDown(OIS::MB_Left))
        {
            mNode->yaw(Degree(-mRotate * arg.state.X.rel), Node::TS_WORLD);
            mNode->pitch(Degree(-mRotate * arg.state.Y.rel), Node::TS_LOCAL);
        }
        if (arg.state.buttonDown(OIS::MB_Right))
        {
            mCamNode->yaw(Degree(-mRotate * arg.state.X.rel), Node::TS_WORLD);
            mCamNode->pitch(Degree(-mRotate * arg.state.Y.rel), Node::TS_LOCAL);
        }
        return true;
    }

    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
    {
        return true;
    }

    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
    {
        return true;
    }

    // JoystickListener
    bool buttonPressed(const OIS::JoyStickEvent &arg, int button) { return true; }
    bool buttonReleased(const OIS::JoyStickEvent &arg, int button) { return true; }
    bool axisMoved(const OIS::JoyStickEvent &arg, int axis) { return true; }

private:
    Application *mApp;

    Root *mRoot;
    OIS::InputManager *mInputManager;
    OIS::Keyboard *mKeyboard;
    OIS::Mouse *mMouse;
    OIS::JoyStick *mJoy;

    Real mRotate;          // The rotate constant
    Real mMove;            // The movement constant

    SceneManager *mSceneMgr;   // The current SceneManager
    SceneNode *mCamNode;   // The SceneNode the camera is currently attached to
    Camera *mCamera;   // The camera

    bool mContinue;        // Whether to continue rendering or not
    Vector3 mCharDirection;     // Value to move in the correct direction
    Vector3 mCowDirection;     // Value to move in the correct direction


    Real mDistance;                  // The distance the object has left to travel
    Vector3 mDirection;              // The direction the object is moving
    Vector3 mDestination;            // The destination the object is moving towards

    AnimationState *mAnimationState; // The current animation state of the object

    Entity *mEntity;                 // The Entity we are animating
    SceneNode *mNode;                // The SceneNode that the Entity is attached to
    std::deque<Vector3> mWalkList;   // The list of points we are walking to

    Real mWalkSpeed;                 // The speed at which the object is moving
};

class Application
{
public:
    void go()
    {
        createRoot();
        defineResources();
        setupRenderSystem();
        createRenderWindow();
        initializeResourceGroups();
        setupScene();
        setupInputSystem();
        createFrameListener();
        startRenderLoop();
    }

    ~Application()
    {
        delete mListener;

        OIS::InputManager::destroyInputSystem(mInputManager);

        delete mRoot;
    }

private:
    Root *mRoot;
    OIS::InputManager *mInputManager;
    ApplicationListener *mListener;

    SceneManager *mSceneMgr;
    Camera *mCamera;
    Viewport *mViewport;

    Entity *mEntity;                // The entity of the object we are animating
    SceneNode *mNode;               // The SceneNode of the object we are moving
    std::deque<Vector3> mWalkList;  // A deque containing the waypoints

    void createRoot()
    {
        mRoot = new Root();
    }

    void defineResources()
    {
        String secName, typeName, archName;
        ConfigFile cf;
        cf.load("resources.cfg");

        ConfigFile::SectionIterator seci = cf.getSectionIterator();
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
            }
        }
    }

    void setupRenderSystem()
    {
        if (!mRoot->restoreConfig() && !mRoot->showConfigDialog())
            throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");

        //// Do not add this to the application
        //RenderSystem *rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
        //                                      // or use "OpenGL Rendering Subsystem"
        //mRoot->setRenderSystem(rs);
        //rs->setConfigOption("Full Screen", "No");
        //rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
    }

    void createRenderWindow()
    {
        mRoot->initialise(true, "Duckinator's FPS Game of Failure");

        //// Do not add this to the application
        //mRoot->initialise(false);
        //HWND hWnd = 0;  // Get the hWnd of the application!
        //NameValuePairList misc;
        //misc["externalWindowHandle"] = StringConverter::toString((int)hWnd);
        //RenderWindow *win = mRoot->createRenderWindow("Main RenderWindow", 800, 600, false, &misc);
    }

    void initializeResourceGroups()
    {
        TextureManager::getSingleton().setDefaultNumMipmaps(5);
        ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    }

    void setupScene()
    {
        mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "Default SceneManager");

        mCamera = mSceneMgr->createCamera("Camera");
        mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);

        SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode", Vector3(0, 200, 400));
        node->attachObject(mCamera);

        mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
        mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

        // create the light
        Light *light = mSceneMgr->createLight("Light1");
        light->setType(Light::LT_POINT);
        light->setPosition(Vector3(250, 150, 250));
        light->setDiffuseColour(ColourValue::Red);
        light->setSpecularColour(ColourValue::Red);

        light = mSceneMgr->createLight("Light2");
        light->setType(Light::LT_POINT);
        light->setPosition(Vector3(-250, 150, 250));
        light->setDiffuseColour(ColourValue::Blue);
        light->setSpecularColour(ColourValue::Blue);

        light = mSceneMgr->createLight("Light3");
        light->setType(Light::LT_POINT);
        light->setPosition(Vector3(0, 150, -250));
        light->setDiffuseColour(ColourValue::Green);
        light->setSpecularColour(ColourValue::Green);

        Plane plane(Vector3::UNIT_Y, 0);
        MeshManager::getSingleton().createPlane("ground",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);
        Entity *ent = mSceneMgr->createEntity("GroundEntity", "ground");
        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
        ent->setMaterialName("Rockwall");
        ent->setCastShadows(false);

        mEntity = mSceneMgr->createEntity("Cow", "cow.mesh");
        mEntity->setCastShadows(true);
        mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CowNode", Vector3(0.0f, 0.0f, 25.0f));
        mNode->attachObject(mEntity);

        // Create the walking list
        mWalkList.push_back(Vector3(550.0f,  0.0f,  50.0f ));
        mWalkList.push_back(Vector3(-100.0f,  0.0f, -200.0f));
    }

    void setupInputSystem()
    {
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;
        OIS::ParamList pl;
        RenderWindow *win = mRoot->getAutoCreatedWindow();

        win->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
        mInputManager = OIS::InputManager::createInputSystem(pl);
    }

    void createFrameListener()
    {
        mListener = new ApplicationListener(mRoot, mInputManager, this, mCamera, mSceneMgr, mNode, mEntity, mWalkList);
        mRoot->addFrameListener(mListener);
    }

    void startRenderLoop()
    {
        mRoot->startRendering();

        //// Do not add this to the application
        //while (mRoot->renderOneFrame())
        //{
        //    // Do some things here, like sleep for x milliseconds or perform other actions.
        //    // However, make sure you call the display update function:
        //    WindowEventUtilities::messagePump();
        //}
    }
};

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
    try
    {
        Application app;
        app.go();
    }
    catch(Exception& e)
    {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
            e.getFullDescription().c_str());
#endif
    }

    return 0;
}
