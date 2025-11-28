#include "MCEngine.hpp"
#include <cmath>
#include <future>
#include <numeric>
#include <thread>
#include <vector>
#include <random>
#include <stdexcept>

namespace mcopt {

    MonteCarloEngine::MonteCarloEngine(
        std::shared_ptr<Payoff> payoff,
        double S0, double T, double r, double sigma)
        : m_payoff(std::move(payoff)), m_S0(S0), m_T(T), m_r(r), m_sigma(sigma) 
    {
        if (!m_payoff) {
            throw std::invalid_argument("Payoff pointer cannot be null.");
        }
        if (m_S0 < 0.0 || m_T < 0.0 || m_sigma < 0.0) {
            throw std::invalid_argument("Invalid market parameters (S0, T, sigma must be >= 0).");
        }
    }

    // Чанк симуляции
    double MonteCarloEngine::runSimulationChunk(double spot, unsigned long long numPaths) const {
        thread_local std::mt19937_64 rng{std::random_device{}()};
        std::normal_distribution<double> dist(0.0, 1.0);

        double sumPayoff = 0.0;
        double drift = (m_r - 0.5 * m_sigma * m_sigma) * m_T;
        double diffusion = m_sigma * std::sqrt(m_T);

        for (unsigned long long i = 0; i < numPaths / 2; ++i) {
            double Z = dist(rng);
            
            double ST_plus = spot * std::exp(drift + diffusion * Z);
            double payoff_plus = (*m_payoff)(ST_plus);

            double ST_minus = spot * std::exp(drift + diffusion * (-Z));
            double payoff_minus = (*m_payoff)(ST_minus);

            sumPayoff += (payoff_plus + payoff_minus);
        }

        if (numPaths % 2 != 0) {
            double Z = dist(rng);
            double ST = spot * std::exp(drift + diffusion * Z);
            sumPayoff += (*m_payoff)(ST);
        }

        return sumPayoff;
    }

    // Обертка для запуска потоков
    double MonteCarloEngine::runSimulationForSpot(double spot, unsigned long long numSimulations) const {
        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 2;

        unsigned long long pathsPerThread = numSimulations / numThreads;
        unsigned long long leftoverPaths = numSimulations % numThreads;

        std::vector<std::future<double>> futures;
        futures.reserve(numThreads);

        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned long long paths = pathsPerThread + (i == 0 ? leftoverPaths : 0);
            futures.push_back(std::async(std::launch::async, 
                &MonteCarloEngine::runSimulationChunk, this, spot, paths));
        }

        double totalSum = 0.0;
        for (auto& f : futures) {
            totalSum += f.get();
        }

        return std::exp(-m_r * m_T) * (totalSum / static_cast<double>(numSimulations));
    }

    double MonteCarloEngine::calculatePrice(unsigned long long numSimulations) const {
        return runSimulationForSpot(m_S0, numSimulations);
    }

    // Метод конечных разностей для Греков
    Greeks MonteCarloEngine::calculateGreeks(unsigned long long numSimulations) {
        // Шаг сдвига (1% от цены или меньше)
        double h = m_S0 * 0.01; 
        
        // 1. Базовая цена
        double price = runSimulationForSpot(m_S0, numSimulations);

        // 2. Цена вверх (S + h)
        double priceUp = runSimulationForSpot(m_S0 + h, numSimulations);

        // 3. Цена вниз (S - h)
        double priceDown = runSimulationForSpot(m_S0 - h, numSimulations);

        Greeks g;
        g.price = price;

        // Central Difference for Delta: (P(S+h) - P(S-h)) / 2h
        g.delta = (priceUp - priceDown) / (2.0 * h);

        // Finite Difference for Gamma: (P(S+h) - 2P(S) + P(S-h)) / h^2
        g.gamma = (priceUp - 2.0 * price + priceDown) / (h * h);

        return g;
    }

} // namespace mcopt