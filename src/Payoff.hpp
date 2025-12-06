#pragma once

#include <algorithm>  // std::max
#include <string>

namespace mcopt {

/**
 * @class Payoff
 * @brief Abstract base class for option payoff strategies.
 *
 * Implements the **Strategy Design Pattern**. This interface allows the
 * Monte Carlo engine to be decoupled from the specific option type (Call, Put, etc.).
 * Derived classes must implement the operator() to calculate the payoff
 * based on the terminal spot price.
 */

class Payoff {
   public:
    Payoff() = default;
    virtual ~Payoff() = default;

    /**
     * @brief Calculates the payoff at expiration.
     *
     * @param spot The spot price of the underlying asset at expiration \f$ S_T \f$.
     * @return The calculated payoff value.
     * @note This method is marked `noexcept` for optimization purposes in tight loops.
     */
    [[nodiscard]] virtual double operator()(double spot) const noexcept = 0;

    /**
     * @brief Returns the name of the payoff type.
     * @return String representation (e.g., "Call", "Put"). Useful for logging/debugging.
     */
    [[nodiscard]] virtual std::string name() const = 0;
};

/**
 * @class PayoffCall
 * @brief European Call Option Payoff.
 *
 * Mathematical formula:
 * \f[
 * Payoff(S_T) = \max(S_T - K, 0)
 * \f]
 * where \f$ K \f$ is the strike price.
 */
class PayoffCall : public Payoff {
   public:
    /**
     * @brief Constructs a Call Option payoff.
     * @param strike The strike price \f$ K \f$.
     */
    explicit PayoffCall(double strike) : m_strike(strike) {}

    [[nodiscard]] double operator()(double spot) const noexcept override;
    [[nodiscard]] std::string name() const override { return "Call"; }

   private:
    double m_strike;
};

/**
 * @class PayoffPut
 * @brief European Put Option Payoff.
 *
 * Mathematical formula:
 * \f[
 * Payoff(S_T) = \max(K - S_T, 0)
 * \f]
 * where \f$ K \f$ is the strike price.
 */
class PayoffPut : public Payoff {
   public:
    /**
     * @brief Constructs a Put Option payoff.
     * @param strike The strike price \f$ K \f$.
     */
    explicit PayoffPut(double strike) : m_strike(strike) {}

    [[nodiscard]] double operator()(double spot) const noexcept override;
    [[nodiscard]] std::string name() const override { return "Put"; }

   private:
    double m_strike;
};

/**
 * @class PayoffAsianCall
 * @brief Arithmetic Asian Call Option.
 * Payoff = max(Average(S) - K, 0)
 */
class PayoffAsianCall : public Payoff {
   public:
    explicit PayoffAsianCall(double strike) : m_strike(strike) {}

    [[nodiscard]] double operator()(double spot) const noexcept override {
        // Здесь spot будет интерпретирован как Average Price
        return std::max(spot - m_strike, 0.0);
    }
    [[nodiscard]] std::string name() const override { return "Asian Call"; }

   private:
    double m_strike;
};

}  // namespace mcopt