#pragma once

#include <string>
#include <vector>

namespace Crystal {
    class FileSystem {
    public:
        using FileList      = std::vector<std::string>;
        using DirectoryList = std::vector<std::string>;

        [[nodiscard]] static constexpr bool IsEmpty(std::string_view str) noexcept;
        [[nodiscard]] static const std::string RemoveIllegalCharacters(std::string_view str) noexcept;
        [[nodiscard]] static const std::string ReplaceIllegalCharacters(std::string_view str, char c = '_') noexcept;

        [[nodiscard]] static const bool CreateDirectory(std::string_view path) noexcept;
        [[nodiscard]] static const bool Delete(std::string_view path) noexcept;
        [[nodiscard]] static const bool Exists(std::string_view path) noexcept;
        [[nodiscard]] static const bool IsDirectory(std::string_view path) noexcept;
        [[nodiscard]] static constexpr bool IsFile(std::string_view path) noexcept;
        [[nodiscard]] static constexpr bool CopyFile(std::string_view src, std::string_view dst) noexcept;
        [[nodiscard]] static const bool HasParentPath(std::string_view path) noexcept;

        [[nodiscard]] static const std::string ReplaceExtension(std::string_view path, std::string_view ext) noexcept;
        [[nodiscard]] static constexpr std::string GetDirectoryFromFilePath(std::string_view path) noexcept;
        [[nodiscard]] static const std::string GetFileNameFromFilePath(std::string_view path) noexcept;
        [[nodiscard]] static constexpr std::string GetExtensionFromFilePath(std::string_view path) noexcept;
        [[nodiscard]] static const std::string GetWorkingDirectory() noexcept;
        [[nodiscard]] static const std::string GetRootDirectory(std::string_view path) noexcept;
        [[nodiscard]] static const std::string GetParentDirectory(std::string_view path) noexcept;
        [[nodiscard]] static const std::string Append(std::string_view first, std::string_view second) noexcept;
        [[nodiscard]] static void ConvertToUnixFriendlyPath(std::string_view path) noexcept;
        [[nodiscard]] static void ConvertToWindowsFriendlyPath(std::string_view path) noexcept;
        [[nodiscard]] static void ConvertToNativeEnginePath(std::string_view path) noexcept;
        [[nodiscard]] static const DirectoryList GetDirectoriesInDirectory(std::string_view path) noexcept;
        [[nodiscard]] static const FileList GetFilesInDirectory(std::string_view path) noexcept;

        [[nodiscard]] static constexpr inline std::string AppendMultiple(auto&&... args) noexcept {
            return (std::ostringstream{} << ... << args).str();
        }
    };
}