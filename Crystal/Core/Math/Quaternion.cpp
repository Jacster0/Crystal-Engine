#include "Quaternion.h"
#include "Matrix.h"

namespace Crystal::Math {
    const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

    constexpr void Quaternion::FromAxes(
        const Vector3& xAxis,
        const Vector3& yAxis,
        const Vector3& zAxis) noexcept
    {
        *this = Matrix(
            xAxis.x, yAxis.x, zAxis.x, 0.0f,
            xAxis.y, yAxis.y, zAxis.y, 0.0f,
            xAxis.z, yAxis.z, zAxis.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        ).GetRotation();
    }
}