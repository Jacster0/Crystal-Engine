#pragma once

#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Crystal::Math {
    class Matrix {
    public:
        constexpr Matrix() noexcept {
            SetIdentity();
        }
        constexpr Matrix(const Matrix& rhs) noexcept
            :
            m00(rhs.m00), m01(rhs.m01), m02(rhs.m02), m03(rhs.m03),
            m10(rhs.m10), m11(rhs.m11), m12(rhs.m12), m13(rhs.m13),
            m20(rhs.m20), m21(rhs.m21), m22(rhs.m22), m23(rhs.m23),
            m30(rhs.m30), m31(rhs.m31), m32(rhs.m32), m33(rhs.m33)
        {}

        constexpr Matrix(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33)
            :
            m00(m00), m01(m01), m02(m02), m03(m03),
            m10(m10), m11(m11), m12(m12), m13(m13),
            m20(m20), m21(m21), m22(m22), m23(m23),
            m30(m30), m31(m31), m32(m32), m33(m33)
        {}

        constexpr Matrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale) noexcept {
            const Matrix mRotation = CreateRotation(rotation);

            m00 = scale.x * mRotation.m00;  m01 = scale.x * mRotation.m01;  m02 = scale.x * mRotation.m02;  m03 = 0.0f;
            m10 = scale.y * mRotation.m10;  m11 = scale.y * mRotation.m11;  m12 = scale.y * mRotation.m12;  m13 = 0.0f;
            m20 = scale.z * mRotation.m20;  m21 = scale.z * mRotation.m21;  m22 = scale.z * mRotation.m22;  m23 = 0.0f;
            m30 = translation.x;            m31 = translation.y;            m32 = translation.z;            m33 = 1.0f;
        }

        ~Matrix() = default;

        [[nodiscard]] static constexpr inline Matrix CreateTranslation(const Vector3& translation) noexcept {
            return {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                translation.x, translation.y, translation.z, 1
            };
        }

        [[nodiscard]] static constexpr inline Matrix CreateRotation(const Quaternion& rotation) noexcept {
            const float num9 = rotation.x * rotation.x;
            const float num8 = rotation.y * rotation.y;
            const float num7 = rotation.z * rotation.z;
            const float num6 = rotation.x * rotation.y;
            const float num5 = rotation.z * rotation.w;
            const float num4 = rotation.z * rotation.x;
            const float num3 = rotation.y * rotation.w;
            const float num2 = rotation.y * rotation.z;
            const float num = rotation.x * rotation.w;

            const float i00 = 1.0f - (2.0f * (num8 + num7));
            const float i01 = 2.0f * (num6 + num5);
            const float i02 = 2.0f * (num4 - num3);
            const float i10 = 2.0f * (num6 - num5);
            const float i11 = 1.0f - (2.0f * (num7 + num9));
            const float i12 = 2.0f * (num2 + num);
            const float i20 = 2.0f * (num4 + num3);
            const float i21 = 2.0f * (num2 - num);
            const float i22 = 1.0f - (2.0f * (num8 + num9));

            return {
                i00,  i01,  i02,  0.0f,
                i10,  i11,  i12,  0.0f,
                i20,  i21,  i22,  0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        }

        [[nodiscard]] static constexpr inline Matrix RotationX(const float angle) {
            const float sinAngle = Math::Sin(angle);
            const float cosAngle = Math::Cos(angle);

            return {
                1, 0, 0, 0,
                0, cosAngle, sinAngle, 0,
                0, -sinAngle, cosAngle, 0,
                0, 0, 0, 1
            };
        }

        [[nodiscard]] static constexpr inline Matrix RotationY(const float angle) {
            const float sinAngle = Math::Sin(angle);
            const float cosAngle = Math::Cos(angle);

            return {
                cosAngle, 0, -sinAngle, 0,
                0,        1,  0,        0,
                sinAngle, 0,  cosAngle, 0,
                0,        0,  0,        1
            };
        }

        [[nodiscard]] static constexpr inline Matrix RotationZ(const float angle) {
            const float sinAngle = Math::Sin(angle);
            const float cosAngle = Math::Cos(angle);

            return {
                cosAngle,  sinAngle, 0, 0,
               -sinAngle,  cosAngle, 0, 0,
                0,         0,        1, 0,
                0,         0,        0, 1
            };
        }

        [[nodiscard]] constexpr Vector3 GetTranslation() const noexcept { return Vector3(m30, m31, m32); }

        [[nodiscard]] constexpr Quaternion GetRotation() const noexcept {
            const Vector3 scale = GetScale();

            // Avoid division by zero
            if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) { return Quaternion(0, 0, 0, 1); }

            // Extract rotation and remove scaling
            Matrix normalized;
            normalized.m00 = m00 / scale.x; normalized.m01 = m01 / scale.x; normalized.m02 = m02 / scale.x; normalized.m03 = 0.0f;
            normalized.m10 = m10 / scale.y; normalized.m11 = m11 / scale.y; normalized.m12 = m12 / scale.y; normalized.m13 = 0.0f;
            normalized.m20 = m20 / scale.z; normalized.m21 = m21 / scale.z; normalized.m22 = m22 / scale.z; normalized.m23 = 0.0f;
            normalized.m30 = 0; normalized.m31 = 0; normalized.m32 = 0; normalized.m33 = 1.0f;

            return RotationMatrixToQuaternion(normalized);
        }

        [[nodiscard]] static constexpr inline Quaternion RotationMatrixToQuaternion(const Matrix& mRot) noexcept {
            Quaternion quaternion;
            float sqrt;
            float half;
            const float scale = mRot.m00 + mRot.m11 + mRot.m22;

            if (scale > 0.0f) {
                sqrt = Math::Sqrt(scale + 1.0f);
                quaternion.w = sqrt * 0.5f;
                sqrt = 0.5f / sqrt;

                quaternion.x = (mRot.m12 - mRot.m21) * sqrt;
                quaternion.y = (mRot.m20 - mRot.m02) * sqrt;
                quaternion.z = (mRot.m01 - mRot.m10) * sqrt;

                return quaternion;
            }

            else if ((mRot.m00 >= mRot.m11) && (mRot.m00 >= mRot.m22)) {
                sqrt = Math::Sqrt(1.0f + mRot.m00 - mRot.m11 - mRot.m22);
                half = 0.5f / sqrt;

                quaternion.x = 0.5f * sqrt;
                quaternion.y = (mRot.m01 + mRot.m10) * half;
                quaternion.z = (mRot.m02 + mRot.m20) * half;
                quaternion.w = (mRot.m12 - mRot.m21) * half;

                return quaternion;
            }

            else if (mRot.m11 > mRot.m22) {
                sqrt = Math::Sqrt(1.0f + mRot.m11 - mRot.m00 - mRot.m22);
                half = 0.5f / sqrt;

                quaternion.x = (mRot.m10 + mRot.m01) * half;
                quaternion.y = 0.5f * sqrt;
                quaternion.z = (mRot.m21 + mRot.m12) * half;
                quaternion.w = (mRot.m20 - mRot.m02) * half;

                return quaternion;
            }

            sqrt = Math::Sqrt(1.0f + mRot.m22 - mRot.m00 - mRot.m11);
            half = 0.5f / sqrt;

            quaternion.x = (mRot.m20 + mRot.m02) * half;
            quaternion.y = (mRot.m21 + mRot.m12) * half;
            quaternion.z = 0.5f * sqrt;
            quaternion.w = (mRot.m01 - mRot.m10) * half;

            return quaternion;
        }

        [[nodiscard]] constexpr Vector3 GetScale() const noexcept {
            const int xs = (Math::Signum(m00 * m01 * m02 * m03) < 0) ? -1 : 1;
            const int ys = (Math::Signum(m10 * m11 * m12 * m13) < 0) ? -1 : 1;
            const int zs = (Math::Signum(m20 * m21 * m22 * m23) < 0) ? -1 : 1;

            return {
                static_cast<float>(xs) * Math::Sqrt(m00 * m00 + m01 * m01 + m02 * m02),
                static_cast<float>(ys) * Math::Sqrt(m10 * m10 + m11 * m11 + m12 * m12),
                static_cast<float>(zs) * Math::Sqrt(m20 * m20 + m21 * m21 + m22 * m22)
            };
        }

        [[nodiscard]] static constexpr inline Matrix CreateScale(float scale)          noexcept { return CreateScale(scale, scale, scale); }
        [[nodiscard]] static constexpr inline Matrix CreateScale(const Vector3& scale) noexcept { return CreateScale(scale.x, scale.y, scale.z); }
        [[nodiscard]] static constexpr inline Matrix CreateScale(float scaleX, float scaleY, float ScaleZ) noexcept {
            return {
                scaleX, 0,      0,      0,
                0,      scaleY, 0,      0,
                0,      0,      ScaleZ, 0,
                0,      0,      0,      1
            };
        }

        [[nodiscard]] static constexpr inline Matrix CreateLookAtLH(
            const Vector3& cameraPosition,
            const Vector3& target,
            const Vector3& up) noexcept
        {
            const Vector3 zAxis = Vector3::Normalize(target - cameraPosition);
            const Vector3 xAxis = Vector3::Normalize(Vector3::Cross(up, zAxis));
            const Vector3 yAxis = Vector3::Cross(zAxis, xAxis);

            return {
                xAxis.x, yAxis.x, zAxis.x, 0,
                xAxis.y, yAxis.y, zAxis.y, 0,
                xAxis.z, yAxis.z, zAxis.z, 0,
                -Vector3::Dot(xAxis, cameraPosition), -Vector3::Dot(yAxis, cameraPosition), -Vector3::Dot(zAxis, cameraPosition), 1.0f
            };
        }

        [[nodiscard]] static constexpr inline Matrix CreatePerspectiveFieldOfViewLH(
            float fov,
            float aspectRatio,
            float nearPlaneDistance,
            float farPlaneDistance) noexcept
        {
            const float yScale = Math::Cot(fov / 2);
            const float xScale = yScale / aspectRatio;

            const float zn = nearPlaneDistance;
            const float zf = farPlaneDistance;

            return {
                xScale, 0, 0, 0,
                0, yScale, 0, 0,
                0, 0, zf / (zf - zn), 1,
                0, 0, -zn * zf / (zf - zn), 0
            };
        }

        [[nodiscard]] constexpr Matrix Transposed() const noexcept { return Transpose(*this); }
        constexpr void Transpose() { *this = Transpose(*this); }

        [[nodiscard]] static constexpr inline Matrix Transpose(const Matrix& matrix) noexcept {
            return {
                matrix.m00, matrix.m10, matrix.m20, matrix.m30,
                matrix.m01, matrix.m11, matrix.m21, matrix.m31,
                matrix.m02, matrix.m12, matrix.m22, matrix.m32,
                matrix.m03, matrix.m13, matrix.m23, matrix.m33
            };
        }

        [[nodiscard]] constexpr Matrix Inversed() const noexcept { return Inverse(*this); }

        [[nodiscard]] static constexpr inline Matrix Inverse(const Matrix& matrix) noexcept {
            float v0 = matrix.m20 * matrix.m31 - matrix.m21 * matrix.m30;
            float v1 = matrix.m20 * matrix.m32 - matrix.m22 * matrix.m30;
            float v2 = matrix.m20 * matrix.m33 - matrix.m23 * matrix.m30;
            float v3 = matrix.m21 * matrix.m32 - matrix.m22 * matrix.m31;
            float v4 = matrix.m21 * matrix.m33 - matrix.m23 * matrix.m31;
            float v5 = matrix.m22 * matrix.m33 - matrix.m23 * matrix.m32;

            float i00 = (v5  * matrix.m11 - v4 * matrix.m12 + v3 * matrix.m13);
            float i10 = -(v5 * matrix.m10 - v2 * matrix.m12 + v1 * matrix.m13);
            float i20 = (v4  * matrix.m10 - v2 * matrix.m11 + v0 * matrix.m13);
            float i30 = -(v3 * matrix.m10 - v1 * matrix.m11 + v0 * matrix.m12);

            const float invDet = 1.0f / (i00 * matrix.m00 + i10 * matrix.m01 + i20 * matrix.m02 + i30 * matrix.m03);

            i00 *= invDet;
            i10 *= invDet;
            i20 *= invDet;
            i30 *= invDet;

            const float i01 = -(v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
            const float i11 = (v5  * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
            const float i21 = -(v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
            const float i31 = (v3  * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

            v0 = matrix.m10 * matrix.m31 - matrix.m11 * matrix.m30;
            v1 = matrix.m10 * matrix.m32 - matrix.m12 * matrix.m30;
            v2 = matrix.m10 * matrix.m33 - matrix.m13 * matrix.m30;
            v3 = matrix.m11 * matrix.m32 - matrix.m12 * matrix.m31;
            v4 = matrix.m11 * matrix.m33 - matrix.m13 * matrix.m31;
            v5 = matrix.m12 * matrix.m33 - matrix.m13 * matrix.m32;

            const float i02 = (v5  * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
            const float i12 = -(v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
            const float i22 = (v4  * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
            const float i32 = -(v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

            v0 = matrix.m21 * matrix.m10 - matrix.m20 * matrix.m11;
            v1 = matrix.m22 * matrix.m10 - matrix.m20 * matrix.m12;
            v2 = matrix.m23 * matrix.m10 - matrix.m20 * matrix.m13;
            v3 = matrix.m22 * matrix.m11 - matrix.m21 * matrix.m12;
            v4 = matrix.m23 * matrix.m11 - matrix.m21 * matrix.m13;
            v5 = matrix.m23 * matrix.m12 - matrix.m22 * matrix.m13;

            const float i03 = -(v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
            const float i13 = (v5  * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
            const float i23 = -(v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
            const float i33 = (v3  * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

            return {
                i00, i01, i02, i03,
                i10, i11, i12, i13,
                i20, i21, i22, i23,
                i30, i31, i32, i33 };
        }

        void constexpr Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) const noexcept
        {
            translation = GetTranslation();
            scale       = GetScale();
            rotation    = GetRotation();
        }

        void constexpr SetIdentity() noexcept
        {
            m00 = 1; m01 = 0; m02 = 0; m03 = 0;
            m10 = 0; m11 = 1; m12 = 0; m13 = 0;
            m20 = 0; m21 = 0; m22 = 1; m23 = 0;
            m30 = 0; m31 = 0; m32 = 0; m33 = 1;
        }

        [[nodiscard]] constexpr Matrix operator*(const Matrix& rhs) const noexcept
        {
            return {
                m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30,
                m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31,
                m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32,
                m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33,
                m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30,
                m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31,
                m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32,
                m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33,
                m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30,
                m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31,
                m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32,
                m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33,
                m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30,
                m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31,
                m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32,
                m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33
            };
        }

        constexpr void operator*=(const Matrix& rhs) { (*this) = (*this) * rhs; }

        [[nodiscard]] constexpr Vector3 operator*(const Vector3& rhs) const noexcept {
            Vector4 vec4;

            vec4.X = (rhs.x * m00) + (rhs.y * m10) + (rhs.z * m20) + m30;
            vec4.Y = (rhs.x * m01) + (rhs.y * m11) + (rhs.z * m21) + m31;
            vec4.Z = (rhs.x * m02) + (rhs.y * m12) + (rhs.z * m22) + m32;
            vec4.W = 1 / ((rhs.x * m03) + (rhs.y * m13) + (rhs.z * m23) + m33);

            return {vec4.X * vec4.W, vec4.Y * vec4.W, vec4.Z * vec4.W};
        }

        [[nodiscard]] constexpr Vector4 operator*(const Vector4& rhs) const noexcept {
            return {
                (rhs.X * m00) + (rhs.Y * m10) + (rhs.Z * m20) + (rhs.W * m30),
                (rhs.X * m01) + (rhs.Y * m11) + (rhs.Z * m21) + (rhs.W * m31),
                (rhs.X * m02) + (rhs.Y * m12) + (rhs.Z * m22) + (rhs.W * m32),
                (rhs.X * m03) + (rhs.Y * m13) + (rhs.Z * m23) + (rhs.W * m33)
            };
        }

        [[nodiscard]] constexpr bool operator==(const Matrix& rhs) const noexcept {
            const float* data_left = Data();
            const float* data_right = rhs.Data();

            for (unsigned i = 0; i < 16; ++i)
            {
                if (data_left[i] != data_right[i])
                    return false;
            }

            return true;
        }

        [[nodiscard]] constexpr bool operator!=(const Matrix& rhs) const { return !(*this == rhs); }

        // Test for equality with another matrix with epsilon.
        [[nodiscard]] constexpr bool Equals(const Matrix& rhs) const {
            const float* data_left = Data();
            const float* data_right = rhs.Data();

            for (unsigned i = 0; i < 16; ++i)
            {
                if (!Math::equals(data_left[i], data_right[i]))
                    return false;
            }

            return true;
        }
        [[nodiscard]] constexpr const float* Data() const noexcept { return &m00; }

        float m00 = 0.0f, m01 = 0.0f, m02 = 0.0f, m03 = 0.0f;
        float m10 = 0.0f, m11 = 0.0f, m12 = 0.0f, m13 = 0.0f;
        float m20 = 0.0f, m21 = 0.0f, m22 = 0.0f, m23 = 0.0f;
        float m30 = 0.0f, m31 = 0.0f, m32 = 0.0f, m33 = 0.0f;
    };
}