#pragma once
#include <cmath>
#include "MathFunctions.h"

namespace Crystal::Math {
	class Vector2 {
	public:
        constexpr Vector2() noexcept : x(0), y(0) {}
        constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}
        constexpr Vector2(int x, int y) noexcept : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
        constexpr Vector2(uint32_t x, uint32_t y) noexcept : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
        constexpr Vector2(float x) noexcept : x(x), y(x) {}
        constexpr Vector2(const Vector2& rhs) noexcept {
            x = rhs.x;
            y = rhs.y;
        }

        [[nodiscard]] constexpr Vector2 operator+(const Vector2& other) const noexcept { return Vector2(x + other.x, y + other.y); }
        constexpr void operator+=(const Vector2& other) noexcept {
            x += other.x;
            y += other.y;
        }

        [[nodiscard]] constexpr Vector2 operator*(const Vector2& other) const noexcept { return Vector2(x * other.x, y * other.y); }
        [[nodiscard]] constexpr Vector2 operator*(const float value)    const noexcept { return Vector2(x * value, y * value); }

        constexpr void operator*=(const Vector2& other) {
            x *= other.x;
            y *= other.y;
        }

        constexpr void operator*=(const float value) {
            x *= value;
            y *= value;
        }
        [[nodiscard]] constexpr Vector2 operator-(const Vector2& other) const noexcept { return Vector2(x - other.x, y - other.y); }
        [[nodiscard]] constexpr Vector2 operator-(const float value)   const noexcept { return Vector2(x - value, y - value); }

        constexpr void operator-=(const Vector2 other) {
            x -= other.x;
            y -= other.y;
        }

        [[nodiscard]] constexpr Vector2 operator/(const Vector2& other) const noexcept { return Vector2(x / other.x, y / other.y); }
        [[nodiscard]] constexpr Vector2 operator/(const float value)    const noexcept { return Vector2(x / value, y / value); }

        constexpr void operator/=(const Vector2& other) noexcept {
            x /= other.x;
            y /= other.y;
        }

        constexpr bool operator==(const Vector2& other) const noexcept { return x == other.x && y == other.y; }
        constexpr bool operator!=(const Vector2& other) const noexcept { return !(*this == other); }

        [[nodiscard]] constexpr float Length()        const noexcept { return Math::Hypot(x, y); }
        [[nodiscard]] constexpr float SquaredLength() const noexcept { return Math::SquaredHypot(x, y); }

        [[nodiscard]] static constexpr inline float Distance(const Vector2& a, const Vector2& b)        noexcept { return (b - a).Length(); }
        [[nodiscard]] static constexpr inline float SquaredDistance(const Vector2& a, const Vector2& b) noexcept { return (b - a).SquaredLength(); }

		float x;
		float y;

		static const Vector2 Zero;
		static const Vector2 one;
	};
}