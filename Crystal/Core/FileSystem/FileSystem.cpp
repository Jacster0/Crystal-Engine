#include "FileSystem.h"
#include "../Utils/StringUtils.h"
#include "../Logging/Logger.h"

#include <filesystem>
#include <algorithm>

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef CopyFile
#undef CopyFile
#endif

using namespace Crystal;

namespace fs = std::filesystem;
namespace ranges = std::ranges;

constexpr bool FileSystem::IsEmpty(std::string_view str) noexcept {
	//First make sure the string is valid
	if (IsEmptyOrWhiteSpace(str)) {
		return false;
	}
	try {
		return fs::is_empty(str);
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what());
	}
	return false;
}

std::string FileSystem::RemoveIllegalCharacters(std::string_view str) noexcept {
	auto legal = std::string(str);

	//Remove characters that are illegal for both names and paths
	std::string illegal = ":?\"<>|";

	//Define our predicate
	auto lambda = [&illegal](char c) { return (illegal.find(c) != std::string::npos); };

	std::erase_if(legal, lambda);

	if (IsDirectory(legal)) {
		return legal;
	}

	//Remove slashes which are illegal characters for file names
	illegal = "\\/";

	std::erase_if(legal, lambda);

	return legal;
}

std::string FileSystem::ReplaceIllegalCharacters(std::string_view str, char c) noexcept {
	auto legal = std::string(str);

	//Replace characters that are illegal for both names and paths
	std::string illegal = ":?\"<>|";

	//Define our predicate
	auto lambda = [&illegal](char c) {
		return (illegal.find(c) != std::string::npos);
	};

	ranges::replace_if(legal, lambda, c);

	if (IsDirectory(legal)) {
		return legal;
	}

	//Replace slashes which are illegal characters for file names
	illegal = "\\/";
	ranges::replace_if(legal, lambda, c);

	return legal;
}

bool FileSystem::CreateDirectory(std::string_view path) noexcept {
	try {
		return fs::create_directories(path);
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what(), " ", path);
	}
	return false;
}

bool FileSystem::Delete(std::string_view path) noexcept {
	try {
		if (Exists(path) && fs::remove_all(path)) {
			return true;
		}
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what(), " ", path);
	}
	return false;
}

bool FileSystem::Exists(std::string_view path) noexcept {
	try {
		return fs::exists(path);
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what(), " ", path);
	}
	return false;
}

bool FileSystem::IsDirectory(std::string_view path) noexcept {
	try {
		if (Exists(path) && fs::is_directory(path)) {
			return true;
		}
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what(), " ", path);
	}
	return false;
}

constexpr bool FileSystem::IsFile(std::string_view path) noexcept {
	if (path.empty()) {
		return false;
	}
	try {
		if (Exists(path) && fs::is_regular_file(path)) {
			return true;
		}
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what(), " ", path);
	}
	return false;
}

constexpr bool FileSystem::CopyFile(std::string_view src, std::string_view dst) noexcept {
	if (src == dst) [[unlikely]] {
		return true;
	}

	if (!Exists(GetDirectoryFromFilePath(dst))) {
		return CreateDirectory(GetDirectoryFromFilePath(dst));
	}

	try {
		return fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what());
		return false;
	}
}

bool FileSystem::HasParentPath(std::string_view path) noexcept {
	return fs::path(path).has_parent_path();
}

std::string FileSystem::ReplaceExtension(std::string_view path, std::string_view ext) noexcept {
	return fs::path(path).replace_extension(ext).string();
}

constexpr std::string FileSystem::GetDirectoryFromFilePath(std::string_view path) noexcept {
	const size_t lastIndex = path.find_last_of("\\/");

	if (lastIndex != std::string::npos) {
		return std::string(path.substr(0, lastIndex + 1));
	}
	return {};
}

std::string FileSystem::GetFileNameFromFilePath(std::string_view path) noexcept {
	return fs::path(path).filename().string();
}

std::string FileSystem::GetExtensionFromFilePath(std::string_view path) noexcept {
	std::string ext;

	try {
		ext = fs::path(path).extension().string();
	}
	catch (const fs::filesystem_error& e) {
		crylog_warning(LogTag::Core, e.what());
	}
	return ext;
}

std::string FileSystem::GetWorkingDirectory() noexcept {
	return fs::current_path().string();
}

std::string FileSystem::GetRootDirectory(std::string_view path) noexcept {
	return fs::path(path).root_directory().string();
}

std::string FileSystem::GetParentDirectory(std::string_view path) noexcept {
	return fs::path(path).parent_path().string();
}

std::string FileSystem::Append(std::string_view first, std::string_view second) noexcept {
	using namespace std::filesystem;

	const auto firstPath  = path(first);
	const auto secondPath = path(second);

	return (firstPath / secondPath).string();
}

void FileSystem::ConvertToUnixFriendlyPath(std::string_view path) noexcept {
	ConvertToNativeEnginePath(path);
}

void FileSystem::ConvertToWindowsFriendlyPath(std::string_view path) noexcept {
	ranges::replace(std::string(path), '/', '\\');
}

void FileSystem::ConvertToNativeEnginePath(std::string_view path) noexcept {
	ranges::replace(std::string(path), '\\', '/');
}

FileSystem::DirectoryList FileSystem::GetDirectoriesInDirectory(std::string_view path) noexcept {
	DirectoryList directories;

	const fs::directory_iterator end;

	for (fs::directory_iterator iter(path); iter != end; iter++) {
		if (!fs::is_directory(iter->status())) {
			continue;
		}
		std::string dir;

		try {
			dir = iter->path().string();
		}
		catch (const std::system_error& e) {
			crylog_warning(LogTag::Core, "Failed to read directory path ", e.what());
		}

		if (!dir.empty()) {
			directories.emplace_back(std::move(dir));
		}
	}

	return directories;
}

FileSystem::FileList FileSystem::GetFilesInDirectory(std::string_view path) noexcept {
	FileList files;

	const fs::directory_iterator end;

	for (fs::directory_iterator iter(path); iter != end; iter++) {
		if (!fs::is_regular_file(iter->status())) {
			continue;
		}
		try {
			files.emplace_back(std::move(iter->path().string()));
		}
		catch (const std::system_error& e) {
			crylog_warning(LogTag::Core, "Failed to read file path ", e.what());
		}
	}

	return files;
}
