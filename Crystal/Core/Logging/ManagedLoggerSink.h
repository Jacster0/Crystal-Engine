#include "Sink.h"
#include "../Networking/NamedPipeClient.h"

namespace Crystal {
	class ManagedLoggerSink final : public ISink {
	public:
		ManagedLoggerSink(const std::string& name) noexcept;
		virtual void Emit(std::string_view message, LogLevel lvl) noexcept override;
	private:
		NamedPipeClient m_namedPipe;

		struct MessageInfo {
			char Message[1024];
			LogLevel Level;
		} m_messageInfo;
	};
}
