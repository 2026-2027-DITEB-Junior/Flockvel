#include "ProjectPanel.h"

#include "imgui.h"

#include <algorithm>
#include <memory>
#include <string>

namespace
{
    constexpr float AssetTileWidth = 92.0f;
    constexpr float AssetTileHeight = 94.0f;
    constexpr float AssetIconSize = 60.0f;

    ImTextureRef GetTextureRef(const std::unique_ptr<clc::GL::Texture>& texture)
    {
        if (!texture)
            return {};

        return ImTextureRef(static_cast<ImTextureID>(texture->GetGLHandle()));
    }

    void DrawAssetTile(const AssetInfo& asset, bool isSelected, const ImTextureRef& icon)
    {
        const std::string name = asset.path.filename().string();
        const std::string id = asset.path.generic_string();

        ImGui::PushID(id.c_str());
        ImGui::InvisibleButton("##asset", ImVec2(AssetTileWidth, AssetTileHeight));

        const bool hovered = ImGui::IsItemHovered();
        const ImVec2 tileMin = ImGui::GetItemRectMin();
        const ImVec2 tileMax = ImGui::GetItemRectMax();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        if (isSelected || hovered)
        {
            const ImU32 color = isSelected ? IM_COL32(50, 112, 175, 180) : IM_COL32(255, 255, 255, 28);
            drawList->AddRectFilled(tileMin, tileMax, color, 4.0f);
        }

        const ImVec2 iconPosition(tileMin.x + (AssetTileWidth - AssetIconSize) * 0.5f, tileMin.y + 5.0f);
        drawList->AddImage(icon, iconPosition,
            ImVec2(iconPosition.x + AssetIconSize, iconPosition.y + AssetIconSize),
            ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        const ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
        const float textX = std::max(tileMin.x + 3.0f, tileMin.x + (AssetTileWidth - textSize.x) * 0.5f);
        drawList->PushClipRect(tileMin, tileMax, true);
        drawList->AddText(ImVec2(textX, tileMin.y + AssetIconSize + 10.0f),
            ImGui::GetColorU32(ImGuiCol_Text), name.c_str());
        drawList->PopClipRect();

        if (hovered)
            ImGui::SetTooltip("%s", asset.path.generic_string().c_str());

        ImGui::PopID();
    }

}

ProjectPanel::ProjectPanel(
    AssetDatabase& assetDatabase,
    const std::filesystem::path& assetsPath)
    : m_assetDatabase(assetDatabase)
    , m_assetsPath(assetsPath.lexically_normal())
    , m_selectedFolder(m_assetsPath)
{}

bool& ProjectPanel::GetShowProject()
{
    return m_showProject;
}

void ProjectPanel::InitializeIcons(const std::filesystem::path& iconDirectory)
{
    ShutdownIcons();

    m_emptyFolderIcon = std::make_unique<clc::GL::Texture>((iconDirectory / "empty_folder.png").string().c_str());
    m_fullFolderIcon = std::make_unique<clc::GL::Texture>((iconDirectory / "full_folder.png").string().c_str());
    m_cppFileIcon = std::make_unique<clc::GL::Texture>((iconDirectory / "file_c++.png").string().c_str());
    m_textFileIcon = std::make_unique<clc::GL::Texture>((iconDirectory / "file_txt.png").string().c_str());
}

void ProjectPanel::ShutdownIcons()
{
    m_emptyFolderIcon.reset();
    m_fullFolderIcon.reset();
    m_cppFileIcon.reset();
    m_textFileIcon.reset();
}

ImTextureRef ProjectPanel::GetAssetIcon(const AssetInfo& asset) const
{
    if (asset.isDirectory)
        return GetTextureRef(m_assetDatabase.GetChildren(asset.path).empty() ? m_emptyFolderIcon : m_fullFolderIcon);

    if (asset.type == AssetType::Script)
        return GetTextureRef(m_cppFileIcon);

    return GetTextureRef(m_textFileIcon);
}

void ProjectPanel::Draw()
{
    if (!m_showProject)
        return;

    ImGui::Begin("Project", &m_showProject);

    constexpr float folderTreeWidth = 220.0f;

    ImGui::BeginChild(
        "FolderTree",
        ImVec2(folderTreeWidth, 0.0f),
        true);

    ImGuiTreeNodeFlags rootFlags =
        ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (m_selectedFolder == m_assetsPath)
        rootFlags |= ImGuiTreeNodeFlags_Selected;

    const bool rootOpen = ImGui::TreeNodeEx(
        "Assets",
        rootFlags);

    if (ImGui::IsItemClicked())
        m_selectedFolder = m_assetsPath;

    if (rootOpen)
    {
        DrawFolderTree(m_assetsPath);
        ImGui::TreePop();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(
        "FolderContents",
        ImVec2(0.0f, 0.0f),
        true);

    DrawFolderContents();

    ImGui::EndChild();

    ImGui::End();
}

void ProjectPanel::DrawFolderTree(
    const std::filesystem::path& folder)
{
    const auto& children = m_assetDatabase.GetChildren(folder);

    for (const AssetInfo& asset : children)
    {
        if (!asset.isDirectory)
            continue;

        const auto& subfolderChildren =
            m_assetDatabase.GetChildren(asset.path);

        const bool hasSubfolders = std::any_of(
            subfolderChildren.begin(),
            subfolderChildren.end(),
            [](const AssetInfo& child)
            {
                return child.isDirectory;
            });

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        if (!hasSubfolders)
            flags |= ImGuiTreeNodeFlags_Leaf;

        if (asset.path == m_selectedFolder)
            flags |= ImGuiTreeNodeFlags_Selected;

        const std::string name = asset.path.filename().string();
        const std::string id = asset.path.generic_string();

        ImGui::PushID(id.c_str());

        const bool nodeOpen = ImGui::TreeNodeEx(
            name.c_str(),
            flags);

        if (ImGui::IsItemClicked())
            m_selectedFolder = asset.path;

        if (nodeOpen)
        {
            if (hasSubfolders)
                DrawFolderTree(asset.path);

            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}

void ProjectPanel::DrawFolderContents()
{
    ImGui::TextUnformatted(m_selectedFolder.filename().string().c_str());

    ImGui::Separator();

    const auto& children =
        m_assetDatabase.GetChildren(m_selectedFolder);

    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const int columnCount = std::max(1, static_cast<int>(availableWidth / AssetTileWidth));

    if (ImGui::BeginTable("AssetGrid", columnCount, ImGuiTableFlags_SizingFixedFit))
    {
        for (const AssetInfo& asset : children)
        {
            ImGui::TableNextColumn();
            DrawAssetTile(asset, asset.path == m_selectedAsset, GetAssetIcon(asset));

            if (ImGui::IsItemClicked())
                m_selectedAsset = asset.path;

            if (asset.isDirectory && ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                m_selectedFolder = asset.path;
            }
        }

        ImGui::EndTable();
    }
}
