#pragma once

#include <string>
#include <vector>

namespace Crystal::FileSystem {
    using FileList = std::vector<std::string>;
    using DirectoryList = std::vector<std::string>;

    [[nodiscard]] constexpr bool IsEmpty(std::string_view str) noexcept;
    [[nodiscard]] const std::string RemoveIllegalCharacters(std::string_view str) noexcept;
    [[nodiscard]] const std::string ReplaceIllegalCharacters(std::string_view str, char c = '_') noexcept;

    [[nodiscard]] const bool CreateDirectory(std::string_view path) noexcept;
    [[nodiscard]] const bool Delete(std::string_view path) noexcept;
    [[nodiscard]] const bool Exists(std::string_view path) noexcept;
    [[nodiscard]] const bool IsDirectory(std::string_view path) noexcept;
    [[nodiscard]] constexpr bool IsFile(std::string_view path) noexcept;
    [[nodiscard]] constexpr bool CopyFile(std::string_view src, std::string_view dst) noexcept;
    [[nodiscard]] const bool HasParentPath(std::string_view path) noexcept;

    [[nodiscard]] const std::string ReplaceExtension(std::string_view path, std::string_view ext) noexcept;
    [[nodiscard]] constexpr std::string GetDirectoryFromFilePath(std::string_view path) noexcept;
    [[nodiscard]] const std::string GetFileNameFromFilePath(std::string_view path) noexcept;
    [[nodiscard]] constexpr std::string GetExtensionFromFilePath(std::string_view path) noexcept;
    [[nodiscard]] const std::string GetWorkingDirectory() noexcept;
    [[nodiscard]] const std::string GetRootDirectory(std::string_view path) noexcept;
    [[nodiscard]] const std::string GetParentDirectory(std::string_view path) noexcept;
    [[nodiscard]] const std::string Append(std::string_view first, std::string_view second) noexcept;
    [[nodiscard]] void ConvertToUnixFriendlyPath(std::string_view path) noexcept;
    [[nodiscard]] void ConvertToWindowsFriendlyPath(std::string_view path) noexcept;
    [[nodiscard]] void ConvertToNativeEnginePath(std::string_view path) noexcept;
    [[nodiscard]] const DirectoryList GetDirectoriesInDirectory(std::string_view path) noexcept;
    [[nodiscard]] const FileList GetFilesInDirectory(std::string_view path) noexcept;

    [[nodiscard]] static constexpr inline std::string AppendMultiple(auto&&... args) noexcept {
        return (std::ostringstream{} << ... << args).str();
    }
}