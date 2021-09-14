#pragma once
#include <limits>
#include <cmath>

//I hate macros, also why did Microsoft decide it was a good
//idea to name a FUNCTION "Rectangle"??????
#ifdef max
#undef max
#endif

namespace Crystal::Math {
    class Rectangle {
    public:
        Rectangle() noexcept
            :
            Left(0),
            Top(0),
            Right(std::numeric_limits<long>::max()),
            Bottom(std::numeric_limits<long>::max())
        {}

        Rectangle(long left, long top, long right, long bottom) noexcept
            :
            Left(left),
            Top(top),
            Right(right),
            Bottom(bottom)
        {}

        auto Width()     const noexcept { return Right - Left; }
        auto Height()    const noexcept { return Bottom - Top; }
        auto Area()      const noexcept { return Width() * Height(); }
        auto Perimeter() const noexcept { return 2 * Width() + 2 * Height(); }

        [[nodiscard]] constexpr bool operator==(const Rectangle& rhs) const noexcept {
            return Left == rhs.Left && Top == rhs.Top && Right == rhs.Right && Bottom == rhs.Bottom;
        }

        [[nodiscard]] constexpr bool operator!=(const Rectangle& rhs) const noexcept {
            return !(*this == rhs);
        }

        long Left;
        long Top;
        long Right;
        long Bottom;
    };
}
