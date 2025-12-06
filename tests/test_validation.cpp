#include <gtest/gtest.h>

#include <memory>

#include "../src/MCEngine.hpp"
#include "../src/Payoff.hpp"

// Проверка исключений и некорректных аргументов

// Тест 1: Валидация входных данных
TEST(EngineValidation, ThrowsOnInvalidInput) {
    auto payoff = std::make_shared<mcopt::PayoffCall>(100.0);
    uint64_t seed = 1;

    // Отрицательное время
    EXPECT_THROW(mcopt::MonteCarloEngine(payoff, 100.0, -1.0, 0.05, 0.2, seed),
                 std::invalid_argument);

    // Отрицательная волатильность
    EXPECT_THROW(mcopt::MonteCarloEngine(payoff, 100.0, 1.0, 0.05, -0.2, seed),
                 std::invalid_argument);

    // Null payoff
    EXPECT_THROW(mcopt::MonteCarloEngine(nullptr, 100.0, 1.0, 0.05, 0.2, seed),
                 std::invalid_argument);
}
