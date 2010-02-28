#include <Ogre.h>
#include <OIS/OIS.h>
//#include <CEGUI/CEGUI.h>
//#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

using namespace Ogre;

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

class ApplicationListener : public FrameListener, public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
    ApplicationListener(Root *root = 0, OIS::InputManager *inputManager = 0)
        : mRoot(root), mInputManager(inputManager)
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

        mContinue = true;
    }

    ~ApplicationListener()
    {
        mInputManager->destroyInputObject(mKeyboard);
        mInputManager->destroyInputObject(mMouse);
        //mInputManager->destroyInputObject(mJoy);
    }

    // FrameListener
    bool frameStarted(const FrameEvent& evt)
    {
        mKeyboard->capture();
        mMouse->capture();
        //mJoy->capture();

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
        default:
            break;
        }
        return mContinue;
    }

    bool keyReleased(const OIS::KeyEvent &arg)
    {
        return true;
    }

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &arg)
    {
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
    Root *mRoot;
    OIS::InputManager *mInputManager;
    OIS::Keyboard *mKeyboard;
    OIS::Mouse *mMouse;
    OIS::JoyStick *mJoy;

    bool mContinue;        // Whether to continue rendering or not
    Vector3 mDirection;     // Value to move in the correct direction
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
        mRoot->initialise(true, "Tutorial Render Window");

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
        SceneManager *mgr = mRoot->createSceneManager(ST_GENERIC, "Default SceneManager");
        Camera *cam = mgr->createCamera("Camera");
        Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(cam);
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
        mListener = new ApplicationListener(mRoot, mInputManager);
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
