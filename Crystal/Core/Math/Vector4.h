#pragma once

#include <cmath>
#include "MathFunctions.h"

namespace Crystal::Math {
    class Vector3;
    class Quaternion;
    class Matrix;
    class Vector4 {
    public:
        constexpr Vector4() noexcept : x(0), y(0), z(0), w(0) {}
        constexpr Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
        constexpr Vector4(float v) noexcept : x(v), y(v), z(v), w(v) {}
        constexpr Vector4(const Vector3& vec3, float w);
        constexpr Vector4(const Vector3& vec3);
        constexpr Vector4(const Quaternion& quat);

        [[nodiscard]] constexpr bool operator ==(const Vector4& rhs) const noexcept {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }
        [[nodiscard]] constexpr bool operator !=(const Vector4& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] constexpr Vector4 operator*(const float value) const noexcept {
            return Vector4(
                x * value,
                y * value,
                z * value,
                w * value
            );
        }

        constexpr void operator*=(const float value) noexcept {
            x *= value;
            y *= value;
            z *= value;
            w *= value;
        }
        constexpr Vector4 operator*(const float val) noexcept {
            return Vector4(
                x * val,
                y * val,
                z * val,
                w * val
            );
        }

        [[nodiscard]] constexpr Vector4 operator*(const Vector4& rhs) const noexcept {
            return Vector4(
                x * rhs.x,
                y * rhs.y,
                z * rhs.z,
                w * rhs.w
            );
        }
        constexpr void operator*=(const Vector4& rhs) noexcept {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            w *= rhs.w;
        }

        [[nodiscard]] constexpr Vector4 operator+(const float v) const noexcept {
            return Vector4(x + v, y + v, z + v, w);
        }

        [[nodiscard]] constexpr Vector4 operator+(const Vector4& rhs) const noexcept {
            return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w);
        }
        constexpr void operator+=(const Vector4& rhs) noexcept {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
        }

        [[nodiscard]] constexpr Vector4 operator/(const Vector4& rhs) const noexcept {
            return Vector4(
                x / rhs.x,
                y / rhs.y,
                z / rhs.z,
                w / rhs.w);
        }
        constexpr void operator/=(const Vector4& rhs) noexcept {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.y;
            w /= rhs.w;
        }
        [[nodiscard]] constexpr Vector4 operator /(const float val) const noexcept {
            return Vector4(x / val, y / val, z / val, w / val);
        }

        [[nodiscard]] constexpr float Length()        const noexcept { return Math::hypot(x, y, z, w); }
        [[nodiscard]] constexpr float SquaredLength() const noexcept { return Math::squared_hypot(x, y, z, w); }

        constexpr void Normalize() noexcept {
            const auto length_squared = SquaredLength();

            if (!Math::equals(length_squared, 1.0f) && length_squared > 0.0f) {
                const auto length_inverted = 1.0f / Math::sqrt(length_squared);
                x *= length_inverted;
                y *= length_inverted;
                z *= length_inverted;
                w *= length_inverted;
            }
        };
        [[nodiscard]] constexpr Vector4 Normalized() const noexcept {
            const auto length_squared = SquaredLength();

            if (!Math::equals(length_squared, 1.0f) && length_squared > 0.0f) {
                const auto length_inverted = 1.0f / Math::sqrt(length_squared);

                return (*this) * length_inverted;
            }
            return *this;
        }

        [[nodiscard]] static constexpr Vector4 Normalize(const Vector4& rhs) noexcept { return rhs.Normalized(); };
        [[nodiscard]] static constexpr Vector4 Negate(const Vector4& vec4)   noexcept { return Vector4(-vec4.x, -vec4.y, -vec4.z, vec4.w); }

        [[nodiscard]] static constexpr Vector4 Rotate(const Vector4& vec4, const Quaternion& rotation) noexcept;
        [[nodiscard]] static constexpr Vector4 TransformNormal(const Vector4& vec4, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector4 TransformCoord(const Vector4& vec4, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector4 Transform(const Vector4& vec4, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector4 MultiplyAdd(const Vector4& first, const Vector4& second, const Vector4& third) noexcept;

        float x;
        float y;
        float z;
        float w;

        static const Vector4 One;
        static const Vector4 Zero;
        static const Vector4 Infinity;
        static const Vector4 InfinityNeg;
    };
}