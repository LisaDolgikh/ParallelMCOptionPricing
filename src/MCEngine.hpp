#pragma once

#include "Payoff.hpp"
#include "Analytical.hpp"
#include <memory>
#include <vector>

namespace mcopt {

    class MonteCarloEngine {
    public:
        MonteCarloEngine(
            std::shared_ptr<Payoff> payoff,
            double S0, double T, double r, double sigma
        );

        // Старый метод (только цена)
        [[nodiscard]] double calculatePrice(unsigned long long numSimulations) const;

        // Новый метод (Цена + Дельта + Гамма)
        [[nodiscard]] Greeks calculateGreeks(unsigned long long numSimulations);

    private:
        std::shared_ptr<Payoff> m_payoff;
        double m_S0;
        double m_T;
        double m_r;
        double m_sigma;

        // Внутренний метод: считает цену для конкретного Spot Price
        // Он приватный, чтобы вызывать внутри calculateGreeks с разным S
        [[nodiscard]] double runSimulationForSpot(double spot, unsigned long long numSimulations) const;

        // Метод для чанка (как раньше, но теперь принимает spot аргументом)
        [[nodiscard]] double runSimulationChunk(double spot, unsigned long long numPaths) const;
    };

} // namespace mcopt