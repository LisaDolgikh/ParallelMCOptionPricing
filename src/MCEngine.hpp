#pragma once

#include "Payoff.hpp"
#include "Analytical.hpp"
#include <memory>
#include <vector>
#include <thread>

namespace mcopt {

    class MonteCarloEngine {
    public:
        MonteCarloEngine(
            std::shared_ptr<Payoff> payoff,
            double S0, double T, double r, double sigma,
            uint64_t seed = 42
        );


        [[nodiscard]] double calculatePrice(unsigned long long numSimulations) const;
        [[nodiscard]] Greeks calculateGreeks(unsigned long long numSimulations);

        // Сеттер для ручного управления потоками (для бенчмарков)
        void setNumThreads(unsigned int threads);

    private:
        std::shared_ptr<Payoff> m_payoff;
        double m_S0;
        double m_T;
        double m_r;
        double m_sigma;
        uint64_t m_seed;

        // Поле для хранения текущего числа потоков
        unsigned int m_numThreads;

        // Внутренний метод: считает цену для конкретного Spot Price
        // Он приватный, чтобы вызывать внутри calculateGreeks с разным S
        [[nodiscard]] double runSimulationForSpot(double spot, unsigned long long numSimulations) const;

        // Метод для чанка
        [[nodiscard]] double runSimulationChunk(double spot, unsigned long long numPaths, unsigned long long chunkIndex) const;
    };

} // namespace mcopt