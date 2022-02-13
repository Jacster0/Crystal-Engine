#pragma once
#include <string>
#include "../Platform/Windows/CrystalWindow.h"

namespace Crystal {
	enum class PipeAccess {
		Read    = 0x80000000,
		Write   = 0x40000000,
		Execute = 0x20000000,
		All     = 0x10000000,
	};

	enum class PipeSharing {
		None   = 0,
		Read   = 0x1,
		Write  = 0x2,
		Delete = 0x4,
	};

	enum class PipeCreationDisposition {
		CreateNew            = 1,
		CreateAlways         = 2,
		OpenExisting         = 3,
		OpenAlways           = 4,
		TruncateOpenExisting = 5,
	};

	class NamedPipeClient {
	public:
		explicit NamedPipeClient(const std::string& name) noexcept;

		void Open() noexcept;
		void Close() const noexcept;
		void Send(const void* buffer, size_t numBytesToWrite) const noexcept;
		void Call(const auto& structure) noexcept;

		void SetPipeAccess(PipeAccess flags) noexcept { m_access = flags; }
		void SetPipeSharing(PipeSharing flags) noexcept { m_shareMode = flags; }
		void SetCreationDisposition(PipeCreationDisposition flags) noexcept { m_creationDisposition = flags; }
	private:
		std::string m_name;
		HANDLE m_hPipe;
		static constexpr auto m_format = R"(\\.\pipe\)";

		PipeAccess m_access = PipeAccess::Write;
		PipeSharing m_shareMode = PipeSharing::None;
		PipeCreationDisposition m_creationDisposition = PipeCreationDisposition::OpenExisting;
	};

	void Crystal::NamedPipeClient::Call(const auto& structure) noexcept {
		Open();
		Send(static_cast<const void*>(&structure), sizeof(structure));
		Close();
	}
}