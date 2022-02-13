#include "MathFunctions.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Crystal::Math {
    constexpr Vector4::Vector4(const Vector3& vec3, float w)
        :
        X(vec3.x),
        Y(vec3.y),
        Z(vec3.z),
        W(w)
    {}

    constexpr Vector4::Vector4(const Vector3& vec3)
        :
        X(vec3.x),
        Y(vec3.y),
        Z(vec3.z),
        W(0.0f)
    {}

    constexpr Vector4::Vector4(const Quaternion& quat)
        :
        X(quat.x),
        Y(quat.y),
        Z(quat.z),
        W(quat.w)
    {}

    constexpr Vector4 Vector4::Rotate(const Vector4& vec4, const Quaternion& rotation) noexcept {
        const Quaternion p{ vec4.X, vec4.Y, vec4.Z, 0 };
        const Quaternion q = rotation.Conjugate();

        const Quaternion& res = Quaternion::Multiply(Quaternion::Multiply(rotation, p), q);

        return Vector4(res);
    }

    constexpr Vector4 Vector4::TransformNormal(const Vector4& vec4, const Matrix& mat) noexcept {
        const Vector4 row0{ mat.m00,mat.m01, mat.m02, mat.m03 };
        const Vector4 row1{ mat.m10,mat.m11, mat.m12, mat.m13 };
        const Vector4 row2{ mat.m20,mat.m21, mat.m22, mat.m23 };

        const auto X = Vector4(vec4.X);
        const auto Y = Vector4(vec4.Y);
        const auto Z = Vector4(vec4.Z);

    	auto result = Z * row2;
        result         = MultiplyAdd(Y, row1, result);
        result         = MultiplyAdd(X, row0, result);
        result.W       = 0;

        return result;
    }

    constexpr Vector4 Vector4::Transform(const Vector4& vec4, const Matrix& mat) noexcept {
        const Vector4 row0{ mat.m00,mat.m01, mat.m02, mat.m03 };
        const Vector4 row1{ mat.m10,mat.m11, mat.m12, mat.m13 };
        const Vector4 row2{ mat.m20,mat.m21, mat.m22, mat.m23 };
        const Vector4 row3{ mat.m30,mat.m31, mat.m32 ,mat.m33 };

        const auto X = Vector4(vec4.X);
        const auto Y = Vector4(vec4.Y);
        const auto Z = Vector4(vec4.Z);

        Vector4 result = MultiplyAdd(Z, row2, row3);
        result         = MultiplyAdd(Y, row1, result);
        result         = MultiplyAdd(X, row0, result);
        result.W       = 0;

        return result;
    }


    constexpr Vector4 Vector4::TransformCoord(const Vector4& vec4, const Matrix& mat) noexcept {
        const Vector4 row0{ mat.m00,mat.m01, mat.m02, mat.m03 };
        const Vector4 row1{ mat.m10,mat.m11, mat.m12, mat.m13 };
        const Vector4 row2{ mat.m20,mat.m21, mat.m22, mat.m23 };
        const Vector4 row3{ mat.m30,mat.m31, mat.m32 ,mat.m33 };

        const auto x = Vector4(vec4.X);
        const auto y = Vector4(vec4.Y);
        const auto z = Vector4(vec4.Z);

        auto result = MultiplyAdd(z, row2, row3);
        result         = MultiplyAdd(y, row1, result);
        result         = MultiplyAdd(x, row0, result);

        const auto w = Vector4(result.W);
        return result / w;
    }

    constexpr Vector4 Vector4::MultiplyAdd(const Vector4& first, const Vector4& second, const Vector4& third) noexcept {
        return { Math::FusedMultiplyAdd(first.X, second.X, third.X),
                 Math::FusedMultiplyAdd(first.Y, second.Y, third.Y),
                 Math::FusedMultiplyAdd(first.Z, second.Z, third.Z),
                 Math::FusedMultiplyAdd(first.W, second.W, third.W) };
    }
}