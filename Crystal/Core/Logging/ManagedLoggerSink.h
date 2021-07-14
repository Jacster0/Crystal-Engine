#include "Sink.h"
#include "../Networking/NamedPipeClient.h"

namespace Crystal {
	class ManagedLoggerSink final : public ISink<ManagedLoggerSink> {
	public:
		ManagedLoggerSink(const std::string& name) noexcept;
		void Emit(std::string_view message, LogLevel lvl, const std::source_location& loc = std::source_location::current()) noexcept;
	private:
		NamedPipeClient m_namedPipe;

		static constexpr auto MAX_MESSAGE_LENGTH = 512u;
		static constexpr auto MAX_FILE_NAME_LENGTH = 260u;
		static constexpr auto MAX_FUNCTION_NAME_LENGTH = 64u;

		struct MessageInfo {
			char Message[MAX_MESSAGE_LENGTH];
			char FileName[MAX_FILE_NAME_LENGTH];
			char FunctionName[MAX_FUNCTION_NAME_LENGTH];
			int Line;
			LogLevel Level;
		} m_messageInfo;
	};
}
