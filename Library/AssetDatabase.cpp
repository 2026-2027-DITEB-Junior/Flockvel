#include "pch.h"
#include "AssetDatabase.h"
#include <algorithm>
#include <cctype>
#include <system_error>

void AssetDatabase::OpenProject(const std::filesystem::path& assetsPath)
{
    m_assetsPath = assetsPath;
    ScanAssets();
}

const std::vector<AssetInfo>& AssetDatabase::GetChildren(
    const std::filesystem::path& folder) const
{
    static const std::vector<AssetInfo> empty;

    const std::string key =
        folder.lexically_normal().generic_string();

    const auto iterator = m_children.find(key);

    if (iterator == m_children.end())
        return empty;

    return iterator->second;
}

AssetType AssetDatabase::GetAssetType(const std::filesystem::path& path) const
{
    std::error_code error;

    if (std::filesystem::is_directory(path, error))
        return AssetType::Folder;

    std::string extension = path.extension().string();

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(std::tolower(character));
        });

    if (extension == ".scene")
        return AssetType::Scene;

    if (extension == ".png" ||
        extension == ".jpg" ||
        extension == ".jpeg" ||
        extension == ".bmp" ||
        extension == ".tga" ||
        extension == ".dds")
    {
        return AssetType::Texture;
    }

    if (extension == ".fbx" ||
        extension == ".obj" ||
        extension == ".gltf" ||
        extension == ".glb")
    {
        return AssetType::Model;
    }

    if (extension == ".wav" ||
        extension == ".mp3" ||
        extension == ".ogg")
    {
        return AssetType::Audio;
    }

    if (extension == ".cpp" ||
        extension == ".h" ||
        extension == ".hpp" ||
        extension == ".cs")
    {
        return AssetType::Script;
    }

    return AssetType::Unknown;
}

void AssetDatabase::ScanAssets()
{
    m_children.clear();

    std::error_code error;

    if (!std::filesystem::exists(m_assetsPath, error) || error)
        return;

    if (!std::filesystem::is_directory(m_assetsPath, error) || error)
        return;

    const std::string rootKey = m_assetsPath.lexically_normal().generic_string();

    m_children[rootKey];

    const auto options = std::filesystem::directory_options::skip_permission_denied;

    std::filesystem::recursive_directory_iterator iterator(m_assetsPath, options, error);

    const std::filesystem::recursive_directory_iterator end;

    while (iterator != end)
    {
        if (error)
        {
            error.clear();
            iterator.increment(error);
            continue;
        }

        const std::filesystem::directory_entry& entry = *iterator;
        const std::filesystem::path path = entry.path();

        const bool isDirectory = entry.is_directory(error);

        if (error)
        {
            error.clear();
            iterator.increment(error);
            continue;
        }

        AssetInfo asset;
        asset.path = path.lexically_normal();
        asset.parentPath = path.parent_path().lexically_normal();
        asset.isDirectory = isDirectory;
        asset.type = GetAssetType(path);

        const std::string parentKey =
            asset.parentPath.generic_string();

        m_children[parentKey].push_back(asset);

        if (isDirectory)
        {
            const std::string folderKey =
                asset.path.generic_string();

            m_children[folderKey];
        }

        iterator.increment(error);
    }

    for (auto& [folder, assets] : m_children)
    {
        std::sort(
            assets.begin(),
            assets.end(),
            [](const AssetInfo& first, const AssetInfo& second)
            {
                if (first.isDirectory != second.isDirectory)
                    return first.isDirectory > second.isDirectory;

                return first.path.filename().string() <
                    second.path.filename().string();
            });
    }
}