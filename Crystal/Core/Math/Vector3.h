#pragma once

#include <cmath>
#include "MathFunctions.h"

namespace Crystal::Math {
    class Vector4;
    class Vector2;
    class Quaternion;
    class Matrix;
    class Vector3 {
    public:
        constexpr Vector3() noexcept : x(0), y(0), z(0) {}
        constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(0) {}
        constexpr Vector3(float v) noexcept : x(v), y(v), z(v) {}
        constexpr Vector3(const Vector3& rhs) noexcept {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
        }
        constexpr Vector3(const Vector2& rhs) noexcept;
        constexpr Vector3(const Vector4& rhs) noexcept;
        constexpr Vector3(float vec[3]) noexcept {
            x = vec[0];
            y = vec[1];
            z = vec[2];
        }

        [[nodiscard]] constexpr Vector3 operator+(const Vector3& rhs) const noexcept {
            return {
                x + rhs.x,
                y + rhs.y,
                z + rhs.z
            };
        }

        [[nodiscard]] constexpr Vector3 operator+(const float v) const noexcept {
            return {x + v, y + v, z + v};
        }

        constexpr void operator+=(Vector3& rhs) noexcept {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
        }

        [[nodiscard]] constexpr Vector3 operator*(const Vector3& rhs) const noexcept {
            return {
                x * rhs.x,
                y * rhs.y,
                z * rhs.z
            };
        }
        [[nodiscard]] constexpr Vector3 operator*(const float value) const  noexcept {
            return {
                x * value,
                y * value,
                z * value
            };
        }

        constexpr void operator*=(const Vector3& other) noexcept {
            x *= other.x;
            y *= other.y;
            z *= other.z;
        }

        constexpr void operator*=(const float value) noexcept {
            x *= value;
            y *= value;
            z *= value;
        }

        [[nodiscard]] constexpr Vector3 operator-(const Vector3& rhs) const noexcept {
            return {
                x - rhs.x,
                y - rhs.y,
                z - rhs.z
            };
        }
        [[nodiscard]] constexpr Vector3 operator-(const float value) const noexcept {
            return {
                x - value,
                y - value,
                z - value
            };
        }

        constexpr void operator-=(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
        }

        [[nodiscard]] constexpr Vector3 operator/(const Vector3& rhs) const noexcept {
            return {
                x / rhs.x,
                y / rhs.y,
                z / rhs.z};
        }
        [[nodiscard]] constexpr Vector3 operator/(const float value) const noexcept {
            return {
                x / value,
                y / value,
                z / value};
        }

        constexpr void operator/=(const Vector3& other) noexcept {
            x /= other.x;
            y /= other.y;
            z /= other.y;
        }

        constexpr bool operator==(const Vector3& rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }
        constexpr bool operator!=(const Vector3& rhs) const noexcept { return !(*this == rhs); }

        constexpr void Normalize() noexcept {
            const auto squaredLength = SquaredLength();

            if (!Math::equals(squaredLength, 1.0f) && squaredLength > 0.0f) {
                const auto invertedLength = 1.0f / Math::Sqrt(squaredLength);

                x *= invertedLength;
                y *= invertedLength;
                z *= invertedLength;
            }
        }
        [[nodiscard]] constexpr Vector3 Normalized() const noexcept {
            const auto squaredLength = SquaredLength();

            if (!Math::equals(squaredLength, 1.0f) && squaredLength > 0.0f) {
                const auto invertedLength = 1.0f / Math::Sqrt(squaredLength);

                return (*this) * invertedLength;
            }
            return *this;
        }

        [[nodiscard]] constexpr float Dot(const Vector3& rhs) const noexcept { return Dot(*this, rhs); }
        [[nodiscard]] constexpr Vector3 Cross(const Vector3& rhs) const noexcept { return Cross(*this, rhs); }

        [[nodiscard]] constexpr float Length()       const noexcept { return Math::Hypot(x,y,z); }
        [[nodiscard]] constexpr float SquaredLength() const noexcept { return Math::SquaredHypot(x, y, z); }

        inline constexpr void ClampMagnitude(float maxLength) noexcept {
            const auto squaredMagnitude = SquaredLength();

            if (squaredMagnitude > maxLength * maxLength) {
                const auto magnitude = Math::Sqrt(squaredMagnitude);

                const auto normalizedX = x / magnitude;
                const auto normalizedY = y / magnitude;
                const auto normalizedZ = z / magnitude;

                x = normalizedX * maxLength;
                y = normalizedY * maxLength;
                z = normalizedZ * maxLength;
            }
        }

        constexpr void Floor() noexcept {
            x = Math::Floor(x);
            y = Math::Floor(y);
            z = Math::Floor(z);
        }

        [[nodiscard]] constexpr Vector3 Abs() const noexcept { return {Math::Abs(x), Math::Abs(y), Math::Abs(z)}; }

        [[nodiscard]] constexpr inline float Distance(const Vector3& rhs)        const noexcept { return (*this - rhs).Length(); }
        [[nodiscard]] constexpr inline float SquaredDistance(const Vector3& rhs) const noexcept { return (*this - rhs).SquaredLength(); }

        [[nodiscard]] static constexpr inline float Dot(const Vector3& v1, const Vector3& v2) noexcept {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }
        [[nodiscard]] static constexpr inline Vector3 Cross(const Vector3& v1, const Vector3& v2) noexcept {
            return {
                v1.y * v2.z - v2.y * v1.z,
                -(v1.x * v2.z - v2.x * v1.z),
                v1.x * v2.y - v2.x * v2.y
            };
        }
        [[nodiscard]] static constexpr inline Vector3 Normalize(const Vector3& rhs)                     noexcept { return rhs.Normalized(); }
        [[nodiscard]] static constexpr inline float Distance(const Vector3& a, const Vector3& b)        noexcept { return (b - a).Length(); }
        [[nodiscard]] static constexpr inline float SquaredDistance(const Vector3& a, const Vector3& b) noexcept { return (b - a).SquaredLength(); }

        [[nodiscard]] static constexpr Vector3 Rotate(const Vector3& vec3, const Quaternion& rotation) noexcept;
        [[nodiscard]] static constexpr Vector3 TransformNormal(const Vector3& vec3, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector3 TransformCoord(const Vector3& vec3, const Matrix& mat) noexcept;
        [[nodiscard]] static constexpr Vector3 MultiplyAdd(const Vector3& first, const Vector3& second, const Vector3& third) noexcept;

        float x;
        float y;
        float z;
    };

    inline Vector3 operator*(float val, const Vector3& rhs) { return rhs * val; }

    static constexpr auto infinity = std::numeric_limits<float>::infinity();

    namespace vec3 {
        constexpr Vector3 Zero(0.0f);
        constexpr Vector3 Left(-1.0f, 0.0f, 0.0f);
        constexpr Vector3 Right(1.0f, 0.0f, 0.0f);
        constexpr Vector3 Up(0.0f, 1.0f, 0.0f);
        constexpr Vector3 Down(0.0f, -1.0f, 0.0f);
        constexpr Vector3 Forward(0.0f, 0.0f, 1.0f);
        constexpr Vector3 Backward(0.0f, 0.0f, -1.0f);
        constexpr Vector3 One(1.0f);
        constexpr Vector3 Infinity(infinity);
        constexpr Vector3 InfinityNeg(-infinity);
    }
}