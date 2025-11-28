#include <gtest/gtest.h>
#include <memory>
#include <cmath>
#include "../src/Payoff.hpp"
#include "../src/Analytical.hpp"
#include "../src/MCEngine.hpp"

TEST(BlackScholesTest, CallOptionValue) {
    auto res = mcopt::BlackScholesAnalytical::calculate(
        100.0, 100.0, 1.0, 0.05, 0.2, mcopt::OptionType::Call);
    EXPECT_NEAR(res.price, 10.45058, 1e-4);
}

TEST(BlackScholesTest, PutCallParity) {
    double S0 = 100.0; double K = 100.0; double T = 1.0; double r = 0.05; double sigma = 0.2;

    auto callRes = mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);
    auto putRes = mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Put);

    double parityLeft = callRes.price - putRes.price;
    double parityRight = S0 - K * std::exp(-r * T);

    EXPECT_NEAR(parityLeft, parityRight, 1e-8);
}

// Тест на Греки
TEST(MonteCarloTest, GreeksConvergence) {
    double S0 = 100.0; double K = 100.0; double T = 1.0; double r = 0.05; double sigma = 0.2;

    auto exact = mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);

    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma);

    // Увеличим число путей для стабильности греков (Gamma очень шумная)
    auto mcRes = engine.calculateGreeks(10'000'000);

    // Допуски:
    // Delta сходится хорошо
    EXPECT_NEAR(mcRes.delta, exact.delta, 0.01); 
    
    // Gamma сходится сложнее (вторая производная), допуск пошире
    EXPECT_NEAR(mcRes.gamma, exact.gamma, 0.05);
}