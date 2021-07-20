#pragma once

namespace Crystal {
    class Viewport {
    public:
        [[nodiscard]] constexpr float AspectRatio() const noexcept { return Width / Height; }

        float TopLeftX;
        float TopLeftY;
        float Width;
        float Height;
        float MinDepth;
        float MaxDepth;
    };
}