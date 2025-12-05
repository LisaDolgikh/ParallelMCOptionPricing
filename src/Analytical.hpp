#pragma once

namespace mcopt {

/**
 * @enum OptionType
 * @brief Defines the type of the option contract.
 */
enum class OptionType {
    Call,  ///< Right to buy the underlying asset.
    Put    ///< Right to sell the underlying asset.
};

/**
 * @struct Greeks
 * @brief Container for the option's sensitivity measures.
 */
struct Greeks {
    double price;  ///< The fair value of the option (PV).
    double delta;  ///< Sensitivity to spot price change ($ \partial V / \partial S $).
    double gamma;  ///< Sensitivity to delta change ($ \partial^2 V / \partial S^2 $).
};

/**
 * @class BlackScholesAnalytical
 * @brief "Golden Source" pricing engine using the Black-Scholes-Merton formula.
 *
 * Used for validating Monte Carlo results. Implements exact solutions for European options:
 * \f[
 * C(S, t) = S N(d_1) - K e^{-rT} N(d_2)
 * \f]
 * where \f$ N(x) \f$ is the CDF of the standard normal distribution.
 */
class BlackScholesAnalytical {
   public:
    /**
     * @brief Calculates Price and Greeks analytically.
     * * @param S Current Spot price ($S_0$).
     * @param K Strike price.
     * @param T Time to maturity in years.
     * @param r Risk-free interest rate (constant).
     * @param sigma Volatility (constant).
     * @param type Option type (Call or Put).
     * @return Greeks structure containing Price, Delta, and Gamma.
     */
    [[nodiscard]] static Greeks calculate(double S, double K, double T, double r, double sigma,
                                          OptionType type);
};

}  // namespace mcopt