#pragma once

/**
 * @file Constants.hpp
 * @brief Глобальные математические константы.
 *
 * Используются `constexpr` для вычислений на этапе компиляции.
 */

namespace mcopt {
namespace math {
/// @brief Число Пи (\f$ \pi \f$) с высокой точностью.
inline constexpr double PI = 3.14159265358979323846;

/// @brief Обратный корень из 2 пи (\f$ 1 / \sqrt{2\pi} \f$).
/// Используется в функции плотности нормального распределения (PDF).
inline constexpr double INV_SQRT_2PI = 0.39894228040143267794;

/// @brief Корень из 2 (\f$ \sqrt{2} \f$).
/// Используется в функции `std::erfc` для CDF.
inline constexpr double SQRT2 = 1.41421356237309504880;
}  // namespace math
}  // namespace mcopt