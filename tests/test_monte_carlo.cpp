#include <gtest/gtest.h>

#include <cmath>
#include <memory>

#include "../src/Analytical.hpp"
#include "../src/MCEngine.hpp"
#include "../src/Payoff.hpp"

// Тест 1: При волатильности близкой к 0, цена MC должна совпадать с внутренней стоимостью
TEST(MonteCarloTest, ZeroVolatilityLimit) {
    double S0 = 110.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.0001;  // Почти ноль

    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    uint64_t seed = 123;
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma, seed);

    double mcPrice = engine.calculatePrice(100'000);

    // Теоретическая цена: S0 - K * exp(-rT) (так как S0 > K)
    double intrinsic = S0 - K * std::exp(-r * T);

    EXPECT_NEAR(mcPrice, intrinsic, 0.05 * std::abs(intrinsic));
}

// Тест 2: Deep Out of the Money (Опцион должен стоить 0)
TEST(MonteCarloTest, DeepOTM) {
    double S0 = 10.0;  // Цена сильно ниже страйка
    double K = 100.0;
    double T = 0.5;
    double r = 0.01;
    double sigma = 0.2;
    uint64_t seed = 123;

    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma, seed);

    double price = engine.calculatePrice(100'000);
    EXPECT_NEAR(price, 0.0, 1e-5);
}

// Тест 3: Тест на Греки
// Проверяем точность вычисления греков
TEST(MonteCarloTest, GreeksConvergence) {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    uint64_t seed = 123;

    auto exact =
        mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);

    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma, seed);

    // Увеличим число путей для стабильности греков (Gamma очень шумная)
    auto mcRes = engine.calculateGreeks(1'000'000);

    // Допуски:
    // gamma шумная, осознанно для него увеличиваем процент
    EXPECT_NEAR(mcRes.delta, exact.delta, 0.05 * std::abs(exact.delta));
    EXPECT_NEAR(mcRes.gamma, exact.gamma, 0.1 * std::abs(exact.gamma));
    //EXPECT_NEAR(mcRes.delta, exact.delta, 0.005);
    //EXPECT_NEAR(mcRes.gamma, exact.gamma, 0.005);
}

// Тест 4: Проверка воспроизводимости
TEST(MonteCarloTest, Reproducibility) {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;

    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    unsigned long long paths = 100'000;

    // Запуск 1: seed = 12345
    mcopt::MonteCarloEngine engine1(payoff, S0, T, r, sigma, 12345);
    double price1 = engine1.calculatePrice(paths);

    // Запуск 2: seed = 12345 (должен совпасть с price1)
    mcopt::MonteCarloEngine engine2(payoff, S0, T, r, sigma, 12345);
    double price2 = engine2.calculatePrice(paths);

    // Запуск 3: seed = 67890 (должен отличаться)
    mcopt::MonteCarloEngine engine3(payoff, S0, T, r, sigma, 67890);
    double price3 = engine3.calculatePrice(paths);

    EXPECT_DOUBLE_EQ(price1, price2) << "Prices with same seed must be identical";
    EXPECT_NE(price1, price3) << "Prices with different seeds must differ";
}

// Тест 5: Put-Call Parity на Monte Carlo движке
// C - P = S - K * exp(-rT)
TEST(MonteCarloTest, PutCallParityMC) {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    unsigned long long paths = 1'000'000;

    uint64_t seed = 123;
    // Фиксируем единый seed для обоих движков

    // Call
    auto callPayoff = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine callEngine(callPayoff, S0, T, r, sigma, seed);
    double callPrice = callEngine.calculatePrice(paths);

    // Put
    auto putPayoff = std::make_shared<mcopt::PayoffPut>(K);
    mcopt::MonteCarloEngine putEngine(putPayoff, S0, T, r, sigma, seed);
    double putPrice = putEngine.calculatePrice(paths);

    double lhs = callPrice - putPrice;
    double rhs = S0 - K * std::exp(-r * T);

    EXPECT_NEAR(lhs, rhs, 0.1);
}

// Тест 6: Сходимость и Antithetic Variates
// Проверяем, что с ростом числа путей ошибка уменьшается (Smoke test)
TEST(MonteCarloTest, ConvergenceCheck) {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    uint64_t seed = 123;

    auto exact =
        mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);  //

    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma, seed);

    double price10k = engine.calculatePrice(1'000);
    double price1M = engine.calculatePrice(1'000'000);

    double err10k = std::abs(price10k - exact.price);
    double err1M = std::abs(price1M - exact.price);

    // Ошибка на 1М путей должна быть меньше, чем на 10к (в большинстве случаев)
    EXPECT_LT(err1M, err10k);
}
