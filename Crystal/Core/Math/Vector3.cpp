#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "MathFunctions.h"

namespace Crystal::Math {
    constexpr Vector3::Vector3(const Vector4& rhs) noexcept
        :
        x(rhs.X),
        y(rhs.Y),
        z(rhs.Z)
    {}

    constexpr Vector3::Vector3(const Vector2& rhs) noexcept
        :
        x(rhs.x),
        y(rhs.y),
        z(0)
    {}

    constexpr Vector3 Vector3::Rotate(const Vector3& vec3, const Quaternion& rotation) noexcept {
        const Quaternion p{ vec3.x, vec3.y, vec3.z, 0 };
        const Quaternion q = rotation.Conjugate();

        const Quaternion& res = rotation * p * q;

        return {res.x, res.y, res.z};
    }

    constexpr Vector3 Vector3::TransformNormal(const Vector3& vec3, const Matrix& mat) noexcept {
        Vector3 row0{ mat.m00,mat.m01, mat.m02 };
        Vector3 row1{ mat.m10,mat.m11, mat.m12 };
        Vector3 row2{ mat.m20,mat.m21, mat.m22 };

        auto X = Vector3(vec3.x);
        auto Y = Vector3(vec3.y);
        auto Z = Vector3(vec3.z);

        Vector3 Result = Z * row2;
        Result         = MultiplyAdd(Y, row1, Result);
        Result         = MultiplyAdd(X, row0, Result);

        return Result;
    }

    constexpr Vector3 Vector3::TransformCoord(const Vector3& vec3, const Matrix& mat) noexcept {
        Vector3 row0{ mat.m00,mat.m01, mat.m02 };
        Vector3 row1{ mat.m10,mat.m11, mat.m12 };
        Vector3 row2{ mat.m20,mat.m21, mat.m22 };
        Vector3 row3{ mat.m20,mat.m21, mat.m22 };

        auto X = Vector3(vec3.x);
        auto Y = Vector3(vec3.y);
        auto Z = Vector3(vec3.z);

        Vector3 Result = MultiplyAdd(Z, row2, row3);
        Result         = MultiplyAdd(Y, row1, Result);
        Result         = MultiplyAdd(X, row0, Result);

        auto W = Vector3(Result.z);
        return Result / W;
    }

    constexpr Vector3 Vector3::MultiplyAdd(const Vector3& first, const Vector3& second, const Vector3& third) noexcept {
        return {Math::FusedMultiplyAdd(first.x, second.x ,third.x),
                       Math::FusedMultiplyAdd(first.y, second.y, third.y),
                       Math::FusedMultiplyAdd(first.z, second.z, third.z)};
    }
}