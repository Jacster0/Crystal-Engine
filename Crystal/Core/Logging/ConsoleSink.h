#pragma once
#include "Sink.h"

namespace Crystal {
    class ConsoleSink final : public ISink {
    public:
        ConsoleSink() noexcept;
        void Emit(std::string_view msg, LogLevel lvl, const std::source_location& loc) noexcept override;
    private:
        std::string m_prefix;
    };
}


