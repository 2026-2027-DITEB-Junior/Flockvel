#pragma once
#include <filesystem>
#include <unordered_map>
#include <vector>

#ifdef IN_LIBRARY
#define ASSET_DATABASE_API __declspec(dllexport)
#else
#define ASSET_DATABASE_API __declspec(dllimport)
#endif

enum class AssetType
{
    Folder,
    Scene,
    Texture,
    Model,
    Audio,
    Script,
    Unknown
};

struct AssetInfo
{
    std::filesystem::path path;       
    std::filesystem::path parentPath; 
    AssetType type;
    bool isDirectory = false;
};

class ASSET_DATABASE_API AssetDatabase
{
public:
    void OpenProject(const std::filesystem::path& assetsPath);

    const std::vector<AssetInfo>& GetChildren(const std::filesystem::path& folder) const;

private:
    void ScanAssets();
    AssetType GetAssetType(const std::filesystem::path& path) const;

    std::filesystem::path m_assetsPath;

    std::unordered_map<std::string, std::vector<AssetInfo>> m_children;
};
