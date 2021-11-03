#pragma once
#include "Vector3.h"

namespace Crystal::Math {
    class Matrix;
    class Quaternion {
    public:
        constexpr Quaternion() noexcept
            :
            x(0),
            y(0),
            z(0),
            w(1)
        {}

        constexpr Quaternion(float x, float y, float z, float w) noexcept
            :
            x(x),
            y(y),
            z(z),
            w(w)
        {}
        constexpr Quaternion& operator =(const Quaternion& rhs) = default;

        static constexpr inline Quaternion FromAngleAxis(float angle, const Vector3& axis) noexcept {
            const auto halfAngle = angle * 0.5f;
            const auto sin       = Math::sin(halfAngle);
            const auto cos       = Math::cos(halfAngle);

            return Quaternion(axis.x * sin, axis.y * sin, axis.z * sin, cos);
        }

        static constexpr inline Quaternion FromPitchYawRoll(float pitch, float yaw, float roll) noexcept {
            const auto halfRoll  = roll * 0.5f;
            const auto halfPitch = pitch * 0.5f;
            const auto halfYaw   = yaw * 0.5f;

            const auto sinRoll  = Math::sin(halfRoll);
            const auto sinPitch = Math::sin(halfPitch);
            const auto sinYaw   = Math::sin(halfYaw);

            const auto cosRoll  = Math::cos(halfRoll);
            const auto cosPitch = Math::cos(halfPitch);
            const auto cosYaw   = Math::cos(halfYaw);

            return Quaternion(
                cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
                sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
                cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
                cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
            );
        }

        static constexpr inline Quaternion FromEulerAngles(const Vector3& rotation) noexcept {
            return FromPitchYawRoll(Math::ToRadians(rotation.y), Math::ToRadians(rotation.x), Math::ToRadians(rotation.z));
        }
        static constexpr inline Quaternion FromEulerAngles(float rotationX, float rotationY, float rotationZ) noexcept {
            return FromPitchYawRoll(Math::ToRadians(rotationY), Math::ToRadians(rotationX), Math::ToRadians(rotationZ));
        }

        static constexpr inline Quaternion Multiply(const Quaternion& q1, const Quaternion& q2) noexcept {
            const float x     = q1.x;
            const float y     = q1.y;
            const float z     = q1.z;
            const float w     = q1.w;
            const float num4  = q2.x;
            const float num3  = q2.y;
            const float num2  = q2.z;
            const float num   = q2.w;
            const float num12 = (y * num2) - (z * num3);
            const float num11 = (z * num4) - (x * num2);
            const float num10 = (x * num3) - (y * num4);
            const float num9  = ((x * num4) + (y * num3)) + (z * num2);

            return Quaternion(
                ((x * num) + (num4 * w)) + num12,
                ((y * num) + (num3 * w)) + num11,
                ((z * num) + (num2 * w)) + num10,
                (w * num) - num9
            );
        }

        constexpr void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) noexcept;

        constexpr auto Conjugate() const noexcept { return Quaternion(-x, -y, -z, w); }
        constexpr auto SquaredLength() const noexcept { return Math::squared_hypot(x, y, z, w); }

        constexpr Vector3 ToEulerAngles() const noexcept {
            //Order of rotations: Z, X, Y
            const float check = 2.0f * (-y * z + w * x);

            if (check < -0.995f) {
                return Vector3(
                    90.0f,
                    0.0f,
                    Math::ToDegrees(Math::atan2(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)))
                );
            }
            else if (check < 0.0995f) {
                return Vector3(
                    90.0f,
                    0.0f,
                    Math::ToDegrees(Math::atan2(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)))
                );
            }
            return Vector3(
                Math::ToDegrees(Math::asin(check)),
                Math::ToDegrees(Math::atan2(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y))),
                Math::ToDegrees(Math::atan2(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)))
            );
        }

        constexpr void Normalize() noexcept {
            const auto squaredLength = SquaredLength();

            if (!Math::equals(squaredLength, 1.0f) && squaredLength > 0.0f) {
                const auto lengthInv = 1 / Math::sqrt(squaredLength);

                x *= lengthInv;
                y *= lengthInv;
                z *= lengthInv;
                w *= lengthInv;
            }
        }

        constexpr Quaternion Normalized() const noexcept {
            const auto length_squared = SquaredLength();

            if (!Math::equals(length_squared, 1.0f) && length_squared > 0.0f) {
                const auto length_inverted = 1.0f / Math::sqrt(length_squared);
                return (*this) * length_inverted;
            }
            else {
                return *this;
            }
        }

        const Quaternion Inverse() const noexcept {
            const auto squaredLength = SquaredLength();

            if (squaredLength == 1.0f) {
                return Conjugate();
            }
            else if (squaredLength >= std::numeric_limits<float>::epsilon()) {
                return Conjugate() * (1.0f / squaredLength);
            }
            return Identity;
        }

        constexpr Quaternion operator*(const Quaternion& rhs) const noexcept { return Multiply(*this, rhs); }

        constexpr void operator*=(const Quaternion& rhs) { *this = Multiply(*this, rhs); }

        constexpr Vector3 operator*(const Vector3& rhs) const noexcept {
            const Vector3 v(x, y, z);
            const Vector3 cross1(v.Cross(rhs));
            const Vector3 cross2(v.Cross(cross1));

            return rhs + 2.0f * (cross1 * w + cross2);
        }

        constexpr Quaternion& operator *=(float rhs) noexcept {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;

            return *this;
        }

        constexpr Quaternion operator *(float rhs) const noexcept { return Quaternion(x * rhs, y * rhs, z * rhs, w * rhs); }

        constexpr bool operator ==(const Quaternion& rhs) const noexcept {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        constexpr bool operator!=(const Quaternion& rhs) const noexcept { return !(*this == rhs); }

        // Test for equality using epsilon
        constexpr bool Equals(const Quaternion& rhs) const noexcept
        {
            return Math::equals(x, rhs.x) && Math::equals(y, rhs.y) && Math::equals(z, rhs.z) && Math::equals(w, rhs.w);
        }

        const auto Pitch() const noexcept { return ToEulerAngles().x; }
        const auto Yaw()   const noexcept { return ToEulerAngles().y; }
        const auto Roll()  const noexcept { return ToEulerAngles().z; }


        float x;
        float y;
        float z;
        float w;

        static const Quaternion Identity;
    };

    inline Vector3 operator*(const Vector3& lhs, const Quaternion& rhs) noexcept { return rhs * lhs; }
    inline Quaternion operator*(float lhs, const Quaternion& rhs) noexcept { return rhs * lhs; }
}