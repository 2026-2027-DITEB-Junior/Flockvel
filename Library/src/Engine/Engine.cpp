#include "pch.h"
#include "Engine.h"
#include <filesystem>
#include <SDL3/SDL.h>
#include "../Graphics/Camera.h"
#include "../Graphics/Renderable.h"
#include "../Controllers/CameraController.h"
#include "../Physics/Dynamics/BoxRb.h"
#include "../Physics/Dynamics/SphereRb.h"
#include "../Debug/DebugSystem.h"
#include "../Audio/AudioSource.h"
#include "../Logic/FlashOnContact.h"
#include "../DeltaTime/DeltaTime.h"
#include "../Game Mechanics/Booster.h"

namespace clc
{
    namespace
    {
        // Buscar hacia arriba permite arrancar desde Visual Studio o desde bin.
        std::filesystem::path FindAssetRoot(std::filesystem::path directory)
        {
            while (!directory.empty())
            {
                std::filesystem::path shader = directory / "data" / "shaders" / "TextureMap.shader";
                if (std::filesystem::exists(shader))
                    return directory;

                std::filesystem::path parent = directory.parent_path();
                if (parent == directory)
                    break;
                directory = parent;
            }
            return std::filesystem::path();
        }
    }

    GameObject* theSpinblade = nullptr;
    bool EngineSystem::Initialize()
    {
        windowSys = &clcWindow;
        windowSys->SetTitle("SpinBlade");
        windowSys->SetSize(1280, 800);
        if (!windowSys->Initialize()) return false;
        m_window = static_cast<SDL_Window*>(windowSys->GetHandle());
        m_glContext = SDL_GL_GetCurrentContext();
        scnMng = &clcSceneManager;
        fct = &clcFactory;
        resMng = &clcResources;
        gfxSys = &clcGraphics;

        std::filesystem::path assetRoot = FindAssetRoot(std::filesystem::current_path());
        if (assetRoot.empty())
        {
            const char* executableDirectory = SDL_GetBasePath();
            if (executableDirectory != nullptr)
                assetRoot = FindAssetRoot(std::filesystem::u8path(executableDirectory));
        }
        if (assetRoot.empty())
            assetRoot = std::filesystem::current_path();

        resMng->SetAssetRoot(assetRoot.u8string());
        if (!fct->Initialize() || !resMng->Initialize() || !scnMng->Initialize() || !gfxSys->Initialize())
        {
            Shutdown();
            return false;
        }
        clcDebug.Initialize();
        clcInputManager.Initialize();
        clcDeltaTime.Initialize();
        m_initialized = true;
        if (!SpecificInitialize())
        {
            Shutdown();
            return false;
        }
        if (!RendersOwnFrame() || true) // TODO: delete TRUE
        {
            //camera
            GameObject* obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec4(0.f, 20.f, 50.f, 1.f);
            Camera* cam = new Camera();
            CameraController* camCtrl = new CameraController();
            obj->AddComp(cam);
            cam->AddToSystem();
            cam->OnCreate();
            obj->AddComp(camCtrl);
            camCtrl->AddToSystem();
            camCtrl->OnCreate();

            //floor
            obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec4(0.f, -5.f, 0.f, 1.f);
            obj->mTransform.mScale = glm::vec4(50.f, 10.f, 50.f, 1.f);
            Renderable* ren = new Renderable();
            BoxRb* box = new BoxRb();
            obj->AddComp(ren);
            obj->AddComp(box);
            box->AddToSystem();
            box->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();

            //wall
            obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec4(30.f, 0.f, 0.f, 1.f);
            obj->mTransform.mScale = glm::vec4(5.f, 20.f, 49.99f, 1.f);
            obj->mTransform.mRotation = glm::vec3(0.0f, 0.0f, -glm::pi<float>() / 3.0f);
            ren = new Renderable();
            box = new BoxRb();
            obj->AddComp(ren);
            obj->AddComp(box);
            box->AddToSystem();
            box->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();
            //wall 2
            obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec4(-25.f, 5.f, 0.f, 1.f);
            obj->mTransform.mScale = glm::vec4(5.f, 10.f, 50.f, 1.f);
            ren = new Renderable();
            box = new BoxRb();
            box->eventType = Flags::Wall;
            obj->AddComp(ren);
            obj->AddComp(box);
            box->AddToSystem();
            box->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();
            //wall 3
            obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec4(0.f, 0.f, -25.f, 1.f);
            obj->mTransform.mScale = glm::vec4(70.f, 20.f, 5.f, 1.f);
            obj->mTransform.mRotation = glm::vec3(-glm::pi<float>() / 3.0f, 0.0f, 0.0f);
            ren = new Renderable();
            box = new BoxRb();
            obj->AddComp(ren);
            obj->AddComp(box);
            box->AddToSystem();
            box->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();

            //enemy
            obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec3(-5.f, 2.5f, 15.f);
            obj->mTransform.mScale = glm::vec3(5.f, 5.f, 5.f);
            ren = new Renderable();
            ren->texPath = "Companion_Cube.jpg";
            box = new BoxRb(BodyType::Static, obj->mTransform.mPosition, obj->mTransform.mScale);
            box->eventType = Flags::Wall;
            FlashOnContact* flash = new FlashOnContact;
            obj->AddComp(flash);
            obj->AddComp(ren);
            obj->AddComp(box);
            box->AddToSystem();
            box->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();
            flash->AddToSystem();
            flash->OnCreate();

            //spinblade
            theSpinblade = new GameObject;
            clcSceneManager.AddObject(theSpinblade);
            theSpinblade->mTransform.mPosition = glm::vec4(10.f, 10.5f, 0.f, 1.f);
            theSpinblade->mTransform.mScale = glm::vec4(2.5f, 2.5f, 2.5f, 1.f);
            theSpinblade->mTransform.mRotation = glm::vec4(0.f, 0.f, glm::pi<float>(), 0.f);
            ren = new Renderable("cone_20_averaged.obj");
            SphereRb* sphere = new SphereRb(BodyType::Dynamic, theSpinblade->mTransform.mPosition, theSpinblade->mTransform.mScale.x / 2.f);
            PlayerController* pc = new PlayerController();
            //sphere->SetMass(1.5f);

            theSpinblade->AddComp(ren);
            theSpinblade->AddComp(sphere);
            theSpinblade->AddComp(pc);
            sphere->AddToSystem();
            sphere->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();
            pc->AddToSystem();
            pc->OnCreate();

            //Booster
            obj = new GameObject;
            clcSceneManager.AddObject(obj);
            obj->mTransform.mPosition = glm::vec3(-10.f, -0.75f, -15.f);
            obj->mTransform.mScale = glm::vec3(10.f, 2.f, 7.5f);
            ren = new Renderable();
            ren->texPath = "Booster.png";
            box = new BoxRb(BodyType::Static, obj->mTransform.mPosition, obj->mTransform.mScale);
            box->eventType = Flags::Trigger;
            flash = new FlashOnContact;
            Booster* boost = new Booster;
            boost->pc = pc;
            obj->AddComp(flash);
            obj->AddComp(ren);
            obj->AddComp(box);
            obj->AddComp(boost);
            box->AddToSystem();
            box->OnCreate();
            ren->AddToSystem();
            ren->OnCreate();
            flash->AddToSystem();
            flash->OnCreate();
            boost->AddToSystem();
            boost->OnCreate();

            AudioSource* audio = new AudioSource("Stardew Valley OST - Summer Natures Crescendo.mp3");
            audio->mLoop = true;
            audio->mVolume = 0.5f;
            audio->mPlayOnStart = true;
            theSpinblade->AddComp(audio);
            audio->OnCreate();
            audio->Start();

            camCtrl->AddPlayer(theSpinblade);


        }
        return true;
    }

    void EngineSystem::Run()
    {
        if (!m_initialized) return;
        m_running = true;
        while (m_running)
        {
            clcDeltaTime.Update();
            if (!RendersOwnFrame())
            {
                clcWindow.Update();
                if (!clcWindow.Exists())
                {
                    m_running = false;
                    break;
                }
                clcInputManager.Update();
                if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED)
                {
                    SDL_Delay(10);
                    continue;
                }
                if (clcSceneManager.Contains(theSpinblade))
                {
                    clcDebug.DrawSphere(theSpinblade->mTransform.mPosition, theSpinblade->mTransform.mScale.x / 2.f, Color(0.95f, 0.4f, 0.6f));

                    clcDebug.DrawLine(theSpinblade->mTransform.mPosition, theSpinblade->mTransform.mPosition + theSpinblade->mForward * 10.f, Color(1.f, 0.f, 0.f));
                    clcDebug.DrawLine(theSpinblade->mTransform.mPosition, theSpinblade->mTransform.mPosition + theSpinblade->mUp * 10.f, Color(0.f, 0.f, 1.f));
                    clcDebug.DrawLine(theSpinblade->mTransform.mPosition, glm::vec3(0), Color(0.f, 0.f, 0.f));

                    clcDebug.DrawCube(theSpinblade->mTransform.mPosition, theSpinblade->mTransform.mScale * 2.f, theSpinblade->mTransform.mRotation / 200.f, Color(0.f, 1.f, 0.f));
                    clcDebug.DrawCubeTwoCorners(glm::vec3(-100.f), glm::vec3(100.f), Color(1.f, 1.f, 1.f));

                }
                gfxSys->Render();
            }
            SpecificRun();
            scnMng->DeleteObjects();
        }
    }

    void EngineSystem::Shutdown()
    {
        if (scnMng != nullptr)
        {
            scnMng->UnloadScene();
            scnMng->DeleteObjects();
        }
        theSpinblade = nullptr;
        if (gfxSys != nullptr)
            gfxSys->Shutdown();
        if (resMng != nullptr)
            resMng->Shutdown();
        // Sound resources and components must be released before the audio system.
        if (m_initialized)
            SpecificShutdown();
        if (windowSys != nullptr)
            windowSys->Close();
        m_window = nullptr;
        m_glContext = nullptr;
        m_initialized = false;
        m_running = false;
    }
}
