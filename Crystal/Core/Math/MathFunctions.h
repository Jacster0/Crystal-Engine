#pragma once

#include <numbers>
#include <concepts>
#include <limits>
#include <bit>
#include <cmath>

//Macros is such a pain in the ass
#ifdef  max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Crystal::Math {
    template<typename T>
    concept Divisble = requires (T x) { x / x; };

    template<typename T>
    concept Number = (std::signed_integral<T> || std::floating_point<T>) && std::_Boolean_testable<T>;

    template<typename T>
    concept Unsigned_Number = std::unsigned_integral<T> && std::_Boolean_testable<T>;

    struct MathConstants {
        static constexpr auto EPSILON    = std::numeric_limits<float>::epsilon();
        static constexpr auto PI_FLOAT   = std::numbers::pi_v<float>;
        static constexpr auto PI         = PI_FLOAT;
        static constexpr auto PI_DIV2    = PI_FLOAT / 2.0f;
        static constexpr auto PI_MUL2    = PI_FLOAT * 2.0f;
        static constexpr auto PI_DOUBLE  = std::numbers::pi_v<double>;
        static constexpr auto PI_MUL2_D  = PI_DOUBLE * 2.0f;
        static constexpr auto PI_DIV2_D  = PI_DOUBLE / 2.0f;
        static constexpr auto TO_DEGREES = 180.0f / MathConstants::PI;
        static constexpr auto TO_RADIANS = MathConstants::PI / 180.0f;
        static constexpr auto MAX_LONG   = std::numeric_limits<long>::max;
    };

    //Check for equality but allow for a small error
    [[nodiscard]] inline constexpr bool equals(Number auto lhs, Number auto rhs, float error = MathConstants::EPSILON) {
        return lhs + error >= rhs && lhs - error <= rhs;
    }

    template<typename T>
    [[nodiscard]] inline constexpr T AlignUpWithMask(T value, size_t mask) noexcept {
        return static_cast<T>((static_cast<size_t>(value + mask) & ~mask));
    }

    template <typename T>
    [[nodiscard]] inline constexpr auto AlignDownWithMask(T value, size_t mask) noexcept {
        return static_cast<T>((static_cast<size_t>(value) & ~mask));
    }

    [[nodiscard]] inline constexpr auto AlignUp(auto value, size_t alignment) noexcept {
        return AlignUpWithMask(value, alignment - 1);
    }

    [[nodiscard]] inline constexpr auto AlignDown(auto value, size_t alignment) noexcept {
        return AlignDownWithMask(value, alignment - 1);
    }

    [[nodiscard]] inline constexpr bool IsAligned(auto value, size_t alignment) noexcept {
        return 0 == (static_cast<size_t>(value) & (alignment - 1));
    }

    [[nodiscard]] constexpr auto DivideByMultiple(Divisble auto value, Divisble auto alignment) {
        return (value + alignment - 1) / alignment;
    }

    [[nodiscard]] inline constexpr Number auto ToDegrees(const Number auto rads) noexcept {
        return rads * MathConstants::TO_DEGREES;
    }

    [[nodiscard]] inline constexpr Number auto ToRadians(const Number auto degrees) noexcept {
        return degrees * MathConstants::TO_RADIANS;
    }

    template<Number T>
    [[nodiscard]] constexpr T FusedMultiplyAdd(T x, T y, T z) noexcept {
        return (x * y) + z;
    }

    template<Number T>
    [[nodiscard]] constexpr T CopySign(T mag, T sgn) {
        if constexpr (!std::is_signed_v<T>) {
            return mag;
        }

        if (mag < 0 && sgn < 0) {
            return mag;
        }
        else if (mag < 0 && sgn > 0) {
            return -1 * mag;
        }
        else if (mag > 0 && sgn < 0) {
            return -1 * mag;
        }
        else if (mag > 0 && sgn > 0) {
            return mag;
        }

        if (sgn == -0) {
            if (mag > 0) {
                return -1 * mag;
            }
            else {
                return mag;
            }
        }

        if (std::numeric_limits<T>::has_infinity()) {
            if (mag == std::numeric_limits<T>::infinity()) {
                if (sgn <= -0) {
                    return -1 * mag;
                }
                else {
                    return mag;
                }
            }
        }
    }

    template<Unsigned_Number T>
    [[nodiscard]] inline constexpr T Signum(T x) {
        return 0 < x;
    }

    template<Number T>
    [[nodiscard]] inline constexpr T Signum(T val) {
        return (0 < val) - (val < 0);
    }

    [[nodiscard]] inline auto GetNearestPow2(std::unsigned_integral auto val, bool roundUp = true) noexcept {
        if (std::has_single_bit(val)) {
            return val;
        }

        const auto next = std::bit_ceil(val);
        const auto prev = std::bit_floor(val);

        const auto n = next - val;
        const auto m = val - prev;

        return (roundUp) ? ((n <= m) ? next : prev) : ((n < m) ? next : prev);
    }

    namespace detail {
        [[nodiscard]] constexpr double sqrt_helper(double val, double curr, double prev) noexcept {
            if (curr == prev) {
                return curr;
            }

            prev = curr;
            curr = 0.5 * (curr + val / curr);

            return sqrt_helper(val, curr, prev);
        }
    }

    [[nodiscard]] constexpr auto Sqrt(std::floating_point auto val) noexcept {
        if (val >= 0 && val < std::numeric_limits<decltype(val)>::infinity()) {
            return static_cast<decltype(val)>(detail::sqrt_helper(val, val, 0));
        }
        return std::numeric_limits<decltype(val)>::quiet_NaN();
    }

    [[nodiscard]] constexpr double Sqrt(std::unsigned_integral auto val) noexcept {
        if (val >= 0 && val < std::numeric_limits<decltype(val)>::max()) {
            return detail::sqrt_helper(val, val, 0);
        }
        return std::numeric_limits<double>::quiet_NaN();
    }

    [[nodiscard]] constexpr int Floor(std::floating_point auto val) noexcept {
        const int converted = static_cast<int>(val);

        if (val < converted) {
            return converted - 1;
        }
        return converted;
    }

    [[nodiscard]] constexpr int Ceil(std::floating_point auto val) noexcept {
        const int converted = static_cast<int>(val);

        if (val > converted) {
            return converted + 1;
        }
        return converted;
    }

    [[nodiscard]] constexpr auto Abs(Number auto val) noexcept {
        if (val < 0) {
            return -val;
        }
        return val;
    }

    template<class... Ts>
    [[nodiscard]] constexpr auto SquaredHypot(Ts... vals) noexcept {
        return ((vals * vals) + ...);
    }

    template<Number... Ts>
    [[nodiscard]] constexpr Number auto Hypot(Ts... vals) noexcept {
        return Math::Sqrt(SquaredHypot(vals...));
    }

    [[nodiscard]] constexpr inline auto Cos(std::floating_point auto degrees) noexcept {
        if (std::is_constant_evaluated()) {
            using Type = decltype(degrees);

            constexpr Type inv2pi = 1.0 / (2.0 * std::numbers::pi_v<Type>);
            constexpr Type a      = 0.5;
            constexpr Type b      = 0.250;
            constexpr Type c      = 16.0;
            constexpr Type d      = 0.225;
            constexpr Type e      = 1.0;

            const Type x = degrees * inv2pi;
            const Type y = b + Math::Floor(x + b);
            const Type z = x - y;
            const Type u = z * c * (Math::Abs(z) - a);
            const Type v = d * u * (u - e);

            return u + v;
        }
        return std::cos(degrees);
    }

    [[nodiscard]] constexpr inline double Cos(int degrees) noexcept {
        return Math::Cos(static_cast<double>(degrees));
    }

    [[nodiscard]] constexpr auto Sin(std::floating_point auto degrees) {
        constexpr auto halfPi = std::numbers::pi / 2;

        return Math::Cos(halfPi - degrees);
    }

    [[nodiscard]] constexpr double Sin(int degrees) {
        return Math::Sin(static_cast<double>(degrees));
    }

    [[nodiscard]] constexpr auto Tan(std::floating_point auto degrees) {
        return Math::Sin(degrees) / Math::Cos(degrees);
    }

    [[nodiscard]] constexpr double Tan(int degrees) {
        return Math::Sin(degrees) / Math::Cos(degrees);
    }

    [[nodiscard]] inline constexpr Number auto Cot(Number auto v) noexcept {
        if (std::is_constant_evaluated()) {
            return Math::Cos(v) / Math::Sin(v);
        }
        return std::cos(v) / std::sin(v);
    }

    //Evil atan approx
    template<std::floating_point T>
    [[nodiscard]] constexpr T Atan(T val) noexcept {
        double result{ 0.0 };

        const T x      = Math::Abs(val);
        const T y      = (x > 1.0) ? 1.0 / x : x;
        const T yPow2  = y * y;
        const T yPow4  = yPow2 * yPow2;
        const T yPow16 = yPow4 * yPow4;

        const T a = Math::FusedMultiplyAdd(-0x1.a7256feb6fc5cp-6 , yPow2 , 0x1.171560ce4a483p-5  );
        const T b = Math::FusedMultiplyAdd(-0x1.2cf5aabc7cef3p-7 , yPow2 , 0x1.162b0b2a3bfcep-6  );
        const T c = Math::FusedMultiplyAdd(b                     , yPow4 , a                     );
        const T d = Math::FusedMultiplyAdd(-0x1.312788dde0801p-10, yPow2 , 0x1.f9690c82492dbp-9  );
        const T e = Math::FusedMultiplyAdd(-0x1.53e1d2a25ff34p-16, yPow2 , 0x1.d3b63dbb65af4p-13 );
        const T f = Math::FusedMultiplyAdd(e                     , yPow4 , d                     );
        const T g = Math::FusedMultiplyAdd(f                     , yPow16, c                     );
        const T h = Math::FusedMultiplyAdd(g                     , yPow2 , -0x1.4f44d841450e1p-5 );
        const T i = Math::FusedMultiplyAdd(h                     , yPow2 , 0x1.7ee3d3f36bb94p-5  );
        const T j = Math::FusedMultiplyAdd(i                     , yPow2 , -0x1.ad32ae04a9fd1p-5 );
        const T k = Math::FusedMultiplyAdd(j                     , yPow2 , 0x1.e17813d66954fp-5  );
        const T l = Math::FusedMultiplyAdd(k                     , yPow2 , -0x1.11089ca9a5bcdp-4 );
        const T m = Math::FusedMultiplyAdd(l                     , yPow2 , 0x1.3b12b2db51738p-4  );
        const T n = Math::FusedMultiplyAdd(l                     , yPow2 , 0x1.3b12b2db51738p-4  );
        const T o = Math::FusedMultiplyAdd(n                     , yPow2 , 0x1.c71c709dfe927p-4  );
        const T p = Math::FusedMultiplyAdd(o                     , yPow2 , -0x1.2492491fa1744p-3 );
        const T q = Math::FusedMultiplyAdd(p                     , yPow2 , 0x1.99999999840d2p-3  );
        const T r = Math::FusedMultiplyAdd(q                     , yPow2 , -0x1.555555555544cp-2 );
        const T s = Math::FusedMultiplyAdd(r * yPow2             , y     , y                     );

        if (x > 1.0) {
            result = 0x1.921fb54442d18p+0 - s;
        }
        else {
            result = s;
        }
        return Math::CopySign(result, val);
    }

    template<std::floating_point T>
    [[nodiscard]] constexpr T Asin(T val) noexcept {
        const T num = Math::Sqrt(1 + (val * val));

        return Math::Atan(val / num);
    }

    template<Number T>
    [[nodiscard]] constexpr double Atan2(T y, T x) {
        if (std::is_constant_evaluated()) {
            constexpr auto pi = std::numbers::pi_v<T>;
            const auto sgn_x  = Math::sgn(x);
            const auto sgn_y  = Math::sgn(y);

            const auto sgn_x_pow2 = sgn_x * sgn_x;
            const auto sgn_y_pow2 = sgn_y * sgn_y;

            const auto a = sgn_x_pow2 * Math::Atan(y / x);
            const auto b = (1 - sgn_x) / 2;
            const auto c = 1 + sgn_y - (sgn_y_pow2);

            return a + b * c * pi;
        }
        return std::atan2(y, x);
    }
}