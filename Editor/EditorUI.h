#pragma once
#include "ProjectPanel.h"
#include "InspectorPanel.h"
#include "Engine/GameObject.h"
#include <cstdint>
#include <string>

namespace clc { class Camera; }

class EditorUI
{
public:
    EditorUI(AssetDatabase& assetDatabase);
    void Initialize();
    void Draw();
    void Shutdown();
    void RequestClose();
    bool WantsToClose() const { return m_close; }
private:
    enum class Action { None, New, Open, Close };
    void Request(Action action, const std::string& path = {});
    void PerformAction();
    bool Save(bool saveAs);
    std::string ChooseFile(bool save);
    void DrawDialogs();
    void CreateObject();
    void AcceptObjectDrop(clc::GameObject* parent);
    void DrawObject(clc::GameObject* object);
    void DrawDockspace();
    void DrawMainMenuBar();
    void DrawHierarchyPanel();
    void DrawInspectorPanel();
    void DrawScenePanel();
    void UpdateSceneCamera();
    glm::vec3 GetSceneCameraDirection() const;
    void DrawProjectPanel();
    void DrawPerformancePanel();
    ProjectPanel m_projectPanel;
    InspectorPanel m_inspectorPanel;
    clc::GameObject* m_selected = nullptr;
	// This camera belongs exclusively to the editor, wont be saved in the scene.
    clc::GameObject* m_sceneCameraObject = nullptr;
    clc::Camera* m_sceneCamera = nullptr;
    float m_cameraYaw = -90.f;   // Horizontak turn.
    float m_cameraPitch = -20.f; // Vertical turn
    bool m_sceneNavigating = false;
    std::uint64_t m_sceneRevision = 0;
	bool m_dirty = false; // There are unchanged modifications in the scene.
    bool m_close = false;
	// Action that waits for save confirmation before executing.
    Action m_pending = Action::None;
    std::string m_pendingPath;
    std::string m_status;
    std::string m_lastDirectory;
    // Movement when hierarchy is applied
    clc::GameObject* m_reparentObject = nullptr;
    clc::GameObject* m_reparentTarget = nullptr;
    clc::GameObject* m_expandObject = nullptr;
    bool m_showHierarchy = true, m_showInspector = true, m_showScene = true;
    bool m_showProject = true, m_showPerformance = true;
    static constexpr int FrameHistorySize = 120;
    float m_frameTimes[FrameHistorySize] = {};
    int m_frameTimeOffset = 0, m_frameTimeCount = 0;
};
