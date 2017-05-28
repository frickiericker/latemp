#ifndef ARITHMETIC_MATH_HPP
#define ARITHMETIC_MATH_HPP

#include <cmath>

namespace math {
    template<typename T>
    T abs(T x)
    {
        return std::fabs(x);
    }

    template<typename T>
    T signum(T x)
    {
        return std::copysign(1.0, x);
    }

    template<int N, typename T>
    T pow(T x)
    {
        T y = x;
        for (int i = 1; i < N; ++i) {
            y *= x;
        }
        return y;
    }
}

#endif
