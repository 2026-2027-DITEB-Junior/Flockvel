#pragma once
#include "imgui.h"
#include "Graphics/GL/AEXTexture.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include <../AssetDatabase.h>

class ProjectPanel
{
public:
    explicit ProjectPanel(AssetDatabase & assetDatabase, const std::filesystem::path& assetsPath);

	void InitializeIcons(const std::filesystem::path& iconDirectory);
	void ShutdownIcons();
	void Draw();

	bool& GetShowProject();

private:
	void DrawFolderTree(const std::filesystem::path& folder);
	void DrawFolderContents();
	ImTextureRef GetAssetIcon(const AssetInfo& asset) const;

	AssetDatabase& m_assetDatabase;

	bool m_showProject = true;

    std::filesystem::path m_assetsPath;
	std::filesystem::path m_selectedFolder;
	std::filesystem::path m_selectedAsset;

	std::unique_ptr<clc::GL::Texture> m_emptyFolderIcon;
	std::unique_ptr<clc::GL::Texture> m_fullFolderIcon;
	std::unique_ptr<clc::GL::Texture> m_cppFileIcon;
	std::unique_ptr<clc::GL::Texture> m_textFileIcon;
};
