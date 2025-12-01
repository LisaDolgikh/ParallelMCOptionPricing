#include <gtest/gtest.h>
#include <cmath>
#include "../src/Analytical.hpp"

// Проверка формулы Блэка-Шоулза

// Тест 1: Проверка эталонного значения
TEST(BlackScholesTest, CallOptionValue) {
    auto res = mcopt::BlackScholesAnalytical::calculate(
        100.0, 100.0, 1.0, 0.05, 0.2, mcopt::OptionType::Call);
    EXPECT_NEAR(res.price, 10.45058, 1e-4);
}


// Тест 2: Put-Call Parity по теоретичсеким формулам
// C - P = S - K * exp(-rT)
TEST(BlackScholesTest, PutCallParityAnalytical) {
    double S0 = 100.0; 
    double K = 100.0; 
    double T = 1.0; 
    double r = 0.05; 
    double sigma = 0.2;
    //Фиксируем единый seed для обоих движков

    auto callRes = mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);
    auto putRes = mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Put);

    double parityLeft = callRes.price - putRes.price;
    double parityRight = S0 - K * std::exp(-r * T);

    EXPECT_NEAR(parityLeft, parityRight, 1e-8);
}


