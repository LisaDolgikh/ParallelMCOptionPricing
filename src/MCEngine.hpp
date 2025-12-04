#pragma once

#include "Payoff.hpp"
#include "Analytical.hpp"
#include <memory>
#include <vector>
#include <thread>

namespace mcopt {

    /**
     * @class MonteCarloEngine
     * @brief High-performance parallel Monte Carlo pricing engine.
     *
     * This class simulates the paths of the underlying asset using **Geometric Brownian Motion (GBM)**
     * under the Risk-Neutral measure:
     * \f[
     * dS_t = r S_t dt + \sigma S_t dW_t
     * \f]
     *
     * Key Features:
     * - **Parallel Execution:** Uses `std::async` and `std::future` for multi-threaded simulation.
     * - **Variance Reduction:** Implements **Antithetic Variates** technique (using \f$ Z \f$ and \f$ -Z \f$).
     * - **Reproducibility:** Supports deterministic execution via seeded RNG (`std::mt19937_64`).
     * - **Greeks Calculation:** Computes Delta and Gamma using Finite Difference Methods.
     */
    class MonteCarloEngine {
    public:
        /**
         * @brief Constructs the Monte Carlo Engine.
         *
         * @param payoff Shared pointer to the option payoff strategy (Call, Put, etc.).
         * @param S0 Initial spot price of the asset.
         * @param T Time to maturity (in years).
         * @param r Risk-free interest rate (decimal, e.g., 0.05 for 5%).
         * @param sigma Volatility of the asset (decimal, e.g., 0.2 for 20%).
         * @param seed Random seed for reproducible results (default: 42).
         * @throws std::invalid_argument If parameters are negative or payoff is null.
         */
        MonteCarloEngine(
            std::shared_ptr<Payoff> payoff,
            double S0, double T, double r, double sigma,
            uint64_t seed = 42
        );
        /**
         * @brief Calculates the option price using Monte Carlo simulation.
         *
         * @param numSimulations Total number of paths to simulate.
         * @return The discounted expected payoff.
         */
        [[nodiscard]] double calculatePrice(unsigned long long numSimulations) const;
        /**
         * @brief Calculates Price, Delta, and Gamma simultaneously.
         *
         * Uses **Finite Difference Method (FDM)** with a small bump \f$ h \f$:
         * - **Delta:** Central difference \f$ \Delta \approx \frac{V(S+h) - V(S-h)}{2h} \f$
         * - **Gamma:** Second order difference \f$ \Gamma \approx \frac{V(S+h) - 2V(S) + V(S-h)}{h^2} \f$
         *
         * @param numSimulations Total number of paths to simulate for each spot point.
         * @return A Greeks structure containing price, delta, and gamma.
         */
        [[nodiscard]] Greeks calculateGreeks(unsigned long long numSimulations);

        /**
         * @brief Manually sets the number of threads for simulation.
         *
         * Useful for benchmarking scalability.
         * @param threads Number of threads. If 0, resets to `std::thread::hardware_concurrency()`.
         */
        void setNumThreads(unsigned int threads);

    private:
        std::shared_ptr<Payoff> m_payoff;
        double m_S0;
        double m_T;
        double m_r;
        double m_sigma;
        uint64_t m_seed;

        /// @brief Current number of threads used for execution.
        unsigned int m_numThreads;
        /**
         * @brief Internal wrapper to run simulation for a specific Spot Price.
         *
         * Handles the distribution of workload across threads.
         * Used internally by `calculatePrice` (with S0) and `calculateGreeks` (with S0 +/- h).
         *
         * @param spot The spot price to start simulation from.
         * @param numSimulations Total number of paths.
         * @return Discounted average payoff.
         */
        [[nodiscard]] double runSimulationForSpot(double spot, unsigned long long numSimulations) const;
        /**
         * @brief Executes a chunk of simulations on a single thread.
         *
         * Implements the **Antithetic Variates** method: for every random draw \f$ Z \f$,
         * it calculates paths for both \f$ Z \f$ and \f$ -Z \f$ to reduce variance.
         *
         * @param spot The starting spot price.
         * @param numPaths Number of paths for this specific chunk.
         * @param chunkIndex Thread index, used to offset the RNG seed for thread-safety.
         * @return Sum of payoffs for this chunk (undiscounted, un-averaged).
         */
        [[nodiscard]] double runSimulationChunk(double spot, unsigned long long numPaths, unsigned long long chunkIndex) const;
    };

} // namespace mcopt