#pragma once

#include <cmath>
#include "MathFunctions.h"

namespace Crystal::Math {
    class Vector3;
    class Quaternion;
    class Matrix;
    class Vector4 {
    public:
        constexpr Vector4() noexcept : X(0), Y(0), Z(0), W(0) {}
        constexpr explicit Vector4(float x, float y, float z, float w) noexcept : X(x), Y(y), Z(z), W(w) {}
        constexpr explicit Vector4(float v) noexcept : X(v), Y(v), Z(v), W(v) {}
        constexpr Vector4(const Vector3& vec3, float w);
        constexpr explicit Vector4(const Vector3& vec3);
        constexpr explicit Vector4(const Quaternion& quat);

        [[nodiscard]] constexpr bool operator ==(const Vector4& rhs) const noexcept {
            return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
        }
        [[nodiscard]] constexpr bool operator !=(const Vector4& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] constexpr Vector4 operator*(const float value) const noexcept {
            return Vector4(
                X * value,
                Y * value,
                Z * value,
                W * value
            );
        }

        constexpr void operator*=(const float value) noexcept {
            X *= value;
            Y *= value;
            Z *= value;
            W *= value;
        }
        constexpr Vector4 operator*(const float val) noexcept {
            return Vector4(
                X * val,
                Y * val,
                Z * val,
                W * val
            );
        }

        [[nodiscard]] constexpr Vector4 operator*(const Vector4& rhs) const noexcept {
            return Vector4(
                X * rhs.X,
                Y * rhs.Y,
                Z * rhs.Z,
                W * rhs.W
            );
        }
        constexpr void operator*=(const Vector4& rhs) noexcept {
            X *= rhs.X;
            Y *= rhs.Y;
            Z *= rhs.Z;
            W *= rhs.W;
        }

        [[nodiscard]] constexpr Vector4 operator+(const float v) const noexcept {
            return Vector4(X + v, Y + v, Z + v, W);
        }

        [[nodiscard]] constexpr Vector4 operator+(const Vector4& rhs) const noexcept {
            return Vector4(X + rhs.X, Y + rhs.Y, Z + rhs.Z, W);
        }
        constexpr void operator+=(const Vector4& rhs) noexcept {
            X += rhs.X;
            Y += rhs.Y;
            Z += rhs.Z;
        }

        [[nodiscard]] constexpr Vector4 operator/(const Vector4& rhs) const noexcept {
            return Vector4(
                X / rhs.X,
                Y / rhs.Y,
                Z / rhs.Z,
                W / rhs.W);
        }
        constexpr void operator/=(const Vector4& rhs) noexcept {
            X /= rhs.X;
            Y /= rhs.Y;
            Z /= rhs.Y;
            W /= rhs.W;
        }
        [[nodiscard]] constexpr Vector4 operator /(const float val) const noexcept {
            return Vector4(X / val, Y / val, Z / val, W / val);
        }

        [[nodiscard]] constexpr float Length()        const noexcept { return Math::Hypot(X, Y, Z, W); }
        [[nodiscard]] constexpr float SquaredLength() const noexcept { return Math::SquaredHypot(X, Y, Z, W); }

        constexpr void Normalize() noexcept {
            const auto length_squared = SquaredLength();

            if (!Math::equals(length_squared, 1.0f) && length_squared > 0.0f) {
                const auto length_inverted = 1.0f / Math::Sqrt(length_squared);
                X *= length_inverted;
                Y *= length_inverted;
                Z *= length_inverted;
                W *= length_inverted;
            }
        };
        [[nodiscard]] constexpr Vector4 Normalized() const noexcept {
            const auto length_squared = SquaredLength();

            if (!Math::equals(length_squared, 1.0f) && length_squared > 0.0f) {
                const auto length_inverted = 1.0f / Math::Sqrt(length_squared);

                return (*this) * length_inverted;
            }
            return *this;
        }

        [[nodiscard]] static constexpr Vector4 Normalize(const Vector4& rhs) noexcept { return rhs.Normalized(); };
        [[nodiscard]] static constexpr Vector4 Negate(const Vector4& vec4)   noexcept { return Vector4(-vec4.X, -vec4.Y, -vec4.Z, vec4.W); }

        [[nodiscard]] static constexpr Vector4 Rotate(const Vector4& vec4, const Quaternion& rotation) noexcept;
        [[nodiscard]] static constexpr Vector4 TransformNormal(const Vector4& vec4, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector4 TransformCoord(const Vector4& vec4, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector4 Transform(const Vector4& vec4, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector4 MultiplyAdd(const Vector4& first, const Vector4& second, const Vector4& third) noexcept;

        float X;
        float Y;
        float Z;
        float W;
    };

    static constexpr auto inf = std::numeric_limits<float>::infinity();

    static constexpr Vector4 One = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    static constexpr Vector4 Zero(0.0f, 0.0f, 0.0f, 0.0f);
    static constexpr Vector4 Infinity(inf, inf, inf, inf);
    static constexpr Vector4 InfinityNeg(-inf, -inf, -inf, -inf);
}