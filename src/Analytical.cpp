#include "Analytical.hpp"

#include <algorithm>
#include <cmath>

#include "Constants.hpp"

namespace mcopt {

// Нормальное кумулятивное распределение (CDF)
// Используем std::erfc для высокой точности
static double norm_cdf(double x) { return 0.5 * std::erfc(-x / math::SQRT2); }

// Плотность вероятности (PDF)
static double norm_pdf(double x) { return math::INV_SQRT_2PI * std::exp(-0.5 * x * x); }

Greeks BlackScholesAnalytical::calculate(double S, double K, double T, double r, double sigma,
                                         OptionType type) {
    if (T <= 0.0) {
        double val = (type == OptionType::Call) ? std::max(S - K, 0.0) : std::max(K - S, 0.0);
        return {val, 0.0, 0.0};  // На экспирации дельта 0 или 1, но упростим
    }

    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    double pdf_d1 = norm_pdf(d1);
    double cdf_d1 = norm_cdf(d1);

    Greeks g;

    if (type == OptionType::Call) {
        g.price = S * cdf_d1 - K * std::exp(-r * T) * norm_cdf(d2);
        g.delta = cdf_d1;
    } else {
        g.price = K * std::exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1);
        g.delta = cdf_d1 - 1.0;
    }

    g.gamma = pdf_d1 / (S * sigma * std::sqrt(T));

    return g;
}

}  // namespace mcopt