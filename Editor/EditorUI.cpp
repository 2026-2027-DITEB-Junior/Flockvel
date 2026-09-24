#include <GL/glew.h>
#include "EditorUI.h"
#include "imgui.h"
#include "Engine/SceneManager.h"
#include "Engine/GameObject.h"
#include "Graphics/GfxSystem.h"
#include "Graphics/Camera.h"
#include "Resources/ResourceManager.h"
#include <windows.h>
#include "src/GameExecution.h"
#include <commdlg.h>
#include <cwchar>

namespace
{
    struct SceneObjectPayload
    {
        clc::GameObject* object;
        // Permite rechazar un arrastre que empezo en una escena ya cerrada.
        std::uint64_t sceneRevision;
    };
}

EditorUI::EditorUI(AssetDatabase& assets)
    : m_projectPanel(assets, std::filesystem::current_path().parent_path() / "Spinblade" / "Assets")
{
}

void EditorUI::Initialize()
{
    m_lastDirectory = clcResources.GetAssetRoot();
    m_sceneRevision = clcSceneManager.GetRevision();
    m_projectPanel.InitializeIcons(std::filesystem::current_path() / "Data");

    m_sceneCameraObject = new clc::GameObject;
    m_sceneCameraObject->mTransform.mPosition = glm::vec3(0.f, 20.f, 50.f);
    m_sceneCamera = new clc::Camera;
    m_sceneCameraObject->AddComp(m_sceneCamera);
    m_sceneCamera->OnCreate();
    m_sceneCamera->nearPlane = 0.1f;
    m_sceneCamera->target = m_sceneCameraObject->mTransform.mPosition
        + GetSceneCameraDirection();
}

void EditorUI::Shutdown()
{
    m_selected = nullptr;
    m_sceneNavigating = false;
    m_projectPanel.ShutdownIcons();
    if (m_sceneCameraObject != nullptr)
    {
        m_sceneCameraObject->Shutdown();
        clcSceneManager.DeleteObjects();
        m_sceneCameraObject = nullptr;
        m_sceneCamera = nullptr;
    }
}

void EditorUI::RequestClose()
{
    Request(Action::Close);
}

void EditorUI::Request(Action action, const std::string& path)
{
    if (m_pending != Action::None)
        return;

    m_pending = action;
    m_pendingPath = path;

    // If there are any chanegs, DrawDialogs will ask befor executing
    if (!m_dirty)
        PerformAction();
}

std::string EditorUI::ChooseFile(bool save)
{
    wchar_t filename[32768] = {};
    if (save)
    {
        std::filesystem::path currentPath = std::filesystem::u8path(clcSceneManager.GetCurrentScenePath());
        std::wstring name = currentPath.filename().wstring();
        if (name.empty())
            name = L"Untitled.json";

        wcsncpy_s(filename, name.c_str(), _TRUNCATE);
    }

    std::wstring directory = std::filesystem::u8path(m_lastDirectory).wstring();
    OPENFILENAMEW dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = GetActiveWindow();
    dialog.lpstrFilter = L"Scene files (*.json;*.scene)\0*.json;*.scene\0All files (*.*)\0*.*\0\0";
    dialog.lpstrFile = filename;
    dialog.nMaxFile = 32768;
    dialog.lpstrInitialDir = directory.c_str();
    dialog.lpstrDefExt = L"json";
    dialog.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

    BOOL selectedFile;
    if (save)
    {
        dialog.Flags |= OFN_OVERWRITEPROMPT;
        selectedFile = GetSaveFileNameW(&dialog);
    }
    else
    {
        dialog.Flags |= OFN_FILEMUSTEXIST;
        selectedFile = GetOpenFileNameW(&dialog);
    }

    if (!selectedFile)
    {
        DWORD error = CommDlgExtendedError();
        if (error != 0)
            m_status = "File dialog error: " + std::to_string(error);

        return std::string();
    }

    std::filesystem::path path(filename);
    m_lastDirectory = path.parent_path().u8string();
    return path.u8string();
}

bool EditorUI::Save(bool saveAs)
{
    std::string path = clcSceneManager.GetCurrentScenePath();
    if (saveAs || path.empty())
        path = ChooseFile(true);

    if (path.empty())
        return false;

    if (!clcSceneManager.SaveScene(path))
    {
        m_status = clcSceneManager.GetLastError();
        return false;
    }

    m_dirty = false;
    m_status = "Saved: " + path;
    return true;
}

void EditorUI::PerformAction()
{
    Action action = m_pending;
    std::string path = m_pendingPath;
    m_pending = Action::None;
    m_pendingPath.clear();

    if (action == Action::Close)
    {
        m_close = true;
        return;
    }

    if (action == Action::Open)
    {
        if (path.empty())
            path = ChooseFile(false);

        if (path.empty())
            return;

        if (!clcSceneManager.LoadScene(path))
        {
            m_status = clcSceneManager.GetLastError();
            return;
        }
        m_status = "Opened: " + path;
    }
    else if (action == Action::New)
    {
        clcSceneManager.UnloadScene();
        m_status = "New scene";
    }

    m_selected = nullptr;
    m_dirty = false;
    m_sceneRevision = clcSceneManager.GetRevision();
}

void EditorUI::DrawDialogs()
{
    if (m_pending != Action::None && m_dirty)
        ImGui::OpenPopup("Unsaved scene");

    if (ImGui::BeginPopupModal("Unsaved scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Save your changes before continuing?");
        if (ImGui::Button("Save"))
        {
            if (Save(false))
            {
                ImGui::CloseCurrentPopup();
                PerformAction();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard"))
        {
            ImGui::CloseCurrentPopup();
            PerformAction();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            m_pending = Action::None;
            m_pendingPath.clear();
            ImGui::CloseCurrentPopup();
        }
        if (!m_status.empty())
            ImGui::TextWrapped("%s", m_status.c_str());

        ImGui::EndPopup();
    }
}

void EditorUI::Draw()
{
    if (m_sceneRevision != clcSceneManager.GetRevision())
    {
        m_selected = nullptr;
        m_expandObject = nullptr;
        m_sceneRevision = clcSceneManager.GetRevision();
        m_dirty = false;
    }
    if (m_selected != nullptr && !clcSceneManager.Contains(m_selected))
        m_selected = nullptr;

    ImGuiIO& io = ImGui::GetIO();
    m_frameTimes[m_frameTimeOffset] = io.DeltaTime * 1000.f;
    m_frameTimeOffset = (m_frameTimeOffset + 1) % FrameHistorySize;
    if (m_frameTimeCount < FrameHistorySize)
        ++m_frameTimeCount;

    if (!io.WantTextInput && m_pending == Action::None && io.KeyCtrl)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_N, false))
            Request(Action::New);
        if (ImGui::IsKeyPressed(ImGuiKey_O, false))
            Request(Action::Open);
        if (ImGui::IsKeyPressed(ImGuiKey_S, false))
            Save(io.KeyShift);
    }

    DrawMainMenuBar();
    DrawDockspace();
    if (m_showHierarchy)
        DrawHierarchyPanel();
    if (m_showInspector)
        DrawInspectorPanel();
    if (m_showScene)
        DrawScenePanel();
    else
        m_sceneNavigating = false;
    if (m_showProject)
        DrawProjectPanel();
    if (m_showPerformance)
        DrawPerformancePanel();
    DrawDialogs();
}

void EditorUI::DrawDockspace()
{
    ImGui::DockSpaceOverViewport(ImGuiID(0), ImGui::GetMainViewport());
}

void EditorUI::DrawMainMenuBar()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New scene", "Ctrl+N"))
            Request(Action::New);
        if (ImGui::MenuItem("Open scene...", "Ctrl+O"))
            Request(Action::Open);
        if (ImGui::MenuItem("Save", "Ctrl+S"))
            Save(false);
        if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S"))
            Save(true);
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))
            RequestClose();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Hierarchy", nullptr, &m_showHierarchy);
        ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
        ImGui::MenuItem("Scene", nullptr, &m_showScene);
        ImGui::MenuItem("Project", nullptr, &m_showProject);
        ImGui::MenuItem("Performance", nullptr, &m_showPerformance);
        ImGui::EndMenu();
    }

    bool executing = clcExe.IsExecuting();
    if (!executing && ImGui::Button("PLAY"))
        clcExe.StartExecution();
    else if (executing && ImGui::Button("STOP"))
        clcExe.StopExecution();

    std::filesystem::path path = std::filesystem::u8path(clcSceneManager.GetCurrentScenePath());
    std::string name = path.filename().u8string();
    if (name.empty())
        name = "Untitled";

    const char* modifiedMark = "";
    if (m_dirty)
        modifiedMark = " *";

    ImGui::Text("  %s%s", name.c_str(), modifiedMark);
    ImGui::EndMainMenuBar();
}

void EditorUI::CreateObject()
{
    clc::GameObject* object = new clc::GameObject;
    object->mName = "GameObject";
    clcSceneManager.AddObject(object);
    m_selected = object;
    m_dirty = true;
}

void EditorUI::AcceptObjectDrop(clc::GameObject* parent)
{
    if (!ImGui::BeginDragDropTarget())
        return;

    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT");
    if (payload != nullptr && payload->DataSize == sizeof(SceneObjectPayload))
    {
        const SceneObjectPayload* dropped = static_cast<const SceneObjectPayload*>(payload->Data);
        if (dropped->sceneRevision == clcSceneManager.GetRevision())
        {
            m_reparentObject = dropped->object;
            m_reparentTarget = parent;
        }
    }
    ImGui::EndDragDropTarget();
}

void EditorUI::DrawObject(clc::GameObject* object)
{
    if (object->IsShutdown())
        return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (object->GetChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf;
    if (object == m_selected)
        flags |= ImGuiTreeNodeFlags_Selected;
    if (object == m_expandObject)
    {
        ImGui::SetNextItemOpen(true);
        m_expandObject = nullptr;
    }

    const char* name = object->mName.c_str();
    if (object->mName.empty())
        name = "GameObject";

    bool open = ImGui::TreeNodeEx(object, flags, "%s", name);
    if (ImGui::IsItemClicked())
        m_selected = object;

    if (ImGui::BeginDragDropSource())
    {
        SceneObjectPayload payload;
        payload.object = object;
        payload.sceneRevision = clcSceneManager.GetRevision();
        ImGui::SetDragDropPayload("SCENE_OBJECT", &payload, sizeof(payload));
        ImGui::TextUnformatted(object->mName.c_str());
        ImGui::EndDragDropSource();
    }
    AcceptObjectDrop(object);

    if (open)
    {
        const std::vector<clc::GameObject*>& children = object->GetChildren();
        for (size_t i = 0; i < children.size(); ++i)
            DrawObject(children[i]);
        ImGui::TreePop();
    }
}

void EditorUI::DrawHierarchyPanel()
{
    m_reparentObject = nullptr;
    m_reparentTarget = nullptr;
    if (ImGui::Begin("Hierarchy", &m_showHierarchy))
    {
        if (ImGui::Button("Add object"))
            CreateObject();
        ImGui::SameLine();
        if (ImGui::Button("Delete") && m_selected != nullptr)
        {
            m_selected->Shutdown();
            m_selected = nullptr;
            m_dirty = true;
        }
        ImGui::Separator();
        ImGui::Selectable("Scene root");
        AcceptObjectDrop(nullptr);

        const std::vector<clc::GameObject*>& roots = clcSceneManager.GetObjects();
        for (size_t i = 0; i < roots.size(); ++i)
            DrawObject(roots[i]);
    }
    ImGui::End();

    // Cambiamos el padre despues de recorrer el arbol, para no modificarlo durante el bucle.
    if (m_reparentObject != nullptr && clcSceneManager.Contains(m_reparentObject))
    {
        bool changed = m_reparentObject->GetParent() != m_reparentTarget;
        if (clcSceneManager.ReparentObject(m_reparentObject, m_reparentTarget))
        {
            m_selected = m_reparentObject;
            m_expandObject = m_reparentTarget;
            if (changed)
                m_dirty = true;
            m_status.clear();
        }
        else
        {
            m_status = clcSceneManager.GetLastError();
        }
    }
}

void EditorUI::DrawInspectorPanel()
{
    m_inspectorPanel.Draw(m_selected, m_showInspector, m_dirty);
}

glm::vec3 EditorUI::GetSceneCameraDirection() const
{
    float yaw = glm::radians(m_cameraYaw);
    float pitch = glm::radians(m_cameraPitch);
    return glm::vec3(glm::cos(yaw) * glm::cos(pitch),
        glm::sin(pitch), glm::sin(yaw) * glm::cos(pitch));
}

void EditorUI::UpdateSceneCamera()
{
    ImGuiIO& io = ImGui::GetIO();
    // only navigate through image
    if (!ImGui::IsItemHovered() || !ImGui::IsMouseDown(ImGuiMouseButton_Right)
        || io.WantTextInput || io.AppFocusLost || io.KeyCtrl || io.KeyAlt
        || io.KeySuper || m_pending != Action::None)
    {
        m_sceneNavigating = false;
        return;
    }

    if (!m_sceneNavigating)
    {
        if (!ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            return;
        m_sceneNavigating = true;
        ImGui::SetWindowFocus();
    }
    else
    {
        //dont use movement from first click
        const float sensitivity = 0.2f;
        m_cameraYaw += io.MouseDelta.x * sensitivity;
        m_cameraPitch -= io.MouseDelta.y * sensitivity;
        if (m_cameraPitch > 89.f) m_cameraPitch = 89.f;
        if (m_cameraPitch < -89.f) m_cameraPitch = -89.f;
    }

    glm::vec3 forward = GetSceneCameraDirection();
    glm::vec3 up(0.f, 1.f, 0.f);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 movement(0.f);
    if (ImGui::IsKeyDown(ImGuiKey_W)) movement += forward;
    if (ImGui::IsKeyDown(ImGuiKey_S)) movement -= forward;
    if (ImGui::IsKeyDown(ImGuiKey_D)) movement += right;
    if (ImGui::IsKeyDown(ImGuiKey_A)) movement -= right;
    if (ImGui::IsKeyDown(ImGuiKey_E)) movement += up;
    if (ImGui::IsKeyDown(ImGuiKey_Q)) movement -= up;

    if (glm::length(movement) > 0.001f)
    {
        float deltaTime = io.DeltaTime;
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        const float speed = 20.f;
        m_sceneCameraObject->mTransform.mPosition +=
            glm::normalize(movement) * speed * deltaTime;
    }
    m_sceneCamera->target = m_sceneCameraObject->mTransform.mPosition + forward;
}

void EditorUI::DrawScenePanel()
{
    if (ImGui::Begin("Scene", &m_showScene))
    {
        if (!m_status.empty())
            ImGui::TextWrapped("%s", m_status.c_str());
        ImGui::TextWrapped("Right mouse: look | Right mouse + WASD: move | Q/E: down/up");

        ImVec2 size = ImGui::GetContentRegionAvail();
        if (size.x > 1 && size.y > 1)
        {
            // save imgui state before drawing scene
            GLint framebuffer;
            GLint viewport[4];
            GLint scissor[4];
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
            glGetIntegerv(GL_VIEWPORT, viewport);
            glGetIntegerv(GL_SCISSOR_BOX, scissor);

            ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;
            int width = static_cast<int>(size.x * scale.x);
            int height = static_cast<int>(size.y * scale.y);
            if (width < 1) width = 1;
            if (width > 4096) width = 4096;
            if (height < 1) height = 1;
            if (height > 4096) height = 4096;

            clcGraphics.RenderScene(width, height, m_sceneCamera);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            ImGui::Image((ImTextureID)(intptr_t)clcGraphics.GetTexture(), size, ImVec2(0,1), ImVec2(1,0));
            UpdateSceneCamera();
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }
        else
            m_sceneNavigating = false;
    }
    else
        m_sceneNavigating = false;
    ImGui::End();
}

void EditorUI::DrawProjectPanel()
{
    m_projectPanel.Draw();
}
void EditorUI::DrawPerformancePanel()
{
	const float fontSize = ImGui::GetFontSize();
	ImGui::SetNextWindowSize(ImVec2(26.0f * fontSize, 16.0f * fontSize), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Performance", &m_showPerformance))
	{
		const ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("FPS: %.1f", io.Framerate);
		ImGui::Text("Average frame: %.2f ms", io.Framerate > 0.0f ? 1000.0f / io.Framerate : 0.0f);
		ImGui::Separator();
		ImGui::Text("Frame time (ms) - last %d frames", m_frameTimeCount);

		// Once full, the next write position is also the oldest sample.
		const int plotOffset = m_frameTimeCount == FrameHistorySize ? m_frameTimeOffset : 0;
		ImGui::PlotLines("##FrameTimes", m_frameTimes, m_frameTimeCount, plotOffset,
			nullptr, 0.0f, FLT_MAX, ImVec2(-1.0f, 6.0f * fontSize));
		ImGui::TextWrapped("Hover over the graph to inspect a frame's time.");
	}
	ImGui::End();
}
