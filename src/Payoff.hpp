#pragma once

#include <algorithm> // std::max
#include <string>

namespace mcopt {

    // Абстрактный базовый класс
    class Payoff {
    public:
        Payoff() = default;
        virtual ~Payoff() = default;

        // const - метод не меняет состояние объекта
        // noexcept - оптимизация (никогда не выбросит исключение (throw))
        [[nodiscard]] virtual double operator()(double spot) const noexcept = 0;
        
        // Для логирования/отладки
        [[nodiscard]] virtual std::string name() const = 0;
    };

    // Call Option: max(S - K, 0)
    class PayoffCall : public Payoff {
    public:
        explicit PayoffCall(double strike) : m_strike(strike) {}

        [[nodiscard]] double operator()(double spot) const noexcept override;
        [[nodiscard]] std::string name() const override { return "Call"; }

    private:
        double m_strike;
    };

    // Put Option: max(K - S, 0)
    class PayoffPut : public Payoff {
    public:
        explicit PayoffPut(double strike) : m_strike(strike) {}

        [[nodiscard]] double operator()(double spot) const noexcept override;
        [[nodiscard]] std::string name() const override { return "Put"; }

    private:
        double m_strike;
    };

} // namespace mcopt