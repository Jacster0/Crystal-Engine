#pragma once

#include <string>
#include <vector>

namespace Crystal::FileSystem {
    using FileList = std::vector<std::string>;
    using DirectoryList = std::vector<std::string>;

    [[nodiscard]] constexpr bool IsEmpty(std::string_view str) noexcept;
    [[nodiscard]] std::string RemoveIllegalCharacters(std::string_view str) noexcept;
    [[nodiscard]] std::string ReplaceIllegalCharacters(std::string_view str, char c = '_') noexcept;

    [[nodiscard]] bool CreateDirectory(std::string_view path) noexcept;
    [[nodiscard]] bool Delete(std::string_view path) noexcept;
    [[nodiscard]] bool Exists(std::string_view path) noexcept;
    [[nodiscard]] bool IsDirectory(std::string_view path) noexcept;
    [[nodiscard]] constexpr bool IsFile(std::string_view path) noexcept;
    [[nodiscard]] constexpr bool CopyFile(std::string_view src, std::string_view dst) noexcept;
    [[nodiscard]] bool HasParentPath(std::string_view path) noexcept;

    [[nodiscard]] std::string ReplaceExtension(std::string_view path, std::string_view ext) noexcept;
    [[nodiscard]] constexpr std::string GetDirectoryFromFilePath(std::string_view path) noexcept;
    [[nodiscard]] std::string GetFileNameFromFilePath(std::string_view path) noexcept;
    [[nodiscard]] constexpr std::string GetExtensionFromFilePath(std::string_view path) noexcept;
    [[nodiscard]] std::string GetWorkingDirectory() noexcept;
    [[nodiscard]] std::string GetRootDirectory(std::string_view path) noexcept;
    [[nodiscard]] std::string GetParentDirectory(std::string_view path) noexcept;
    [[nodiscard]] std::string Append(std::string_view first, std::string_view second) noexcept;
	void ConvertToUnixFriendlyPath(std::string_view path) noexcept;
	void ConvertToWindowsFriendlyPath(std::string_view path) noexcept;
    void ConvertToNativeEnginePath(std::string_view path) noexcept;
    [[nodiscard]] DirectoryList GetDirectoriesInDirectory(std::string_view path) noexcept;
    [[nodiscard]] FileList GetFilesInDirectory(std::string_view path) noexcept;

    [[nodiscard]] static constexpr std::string AppendMultiple(auto&&... args) noexcept {
        return (std::ostringstream{} << ... << args).str();
    }
}