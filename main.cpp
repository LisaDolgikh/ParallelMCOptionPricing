#include <iostream>
#include <memory>
#include <iomanip>
#include "src/Payoff.hpp"
#include "src/Analytical.hpp"
#include "src/MCEngine.hpp"

int main() {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;

    std::cout << "=== Option Pricing Engine ===" << std::endl;
    std::cout << "S0=" << S0 << " K=" << K << " T=" << T << std::endl;

    // 1. Exact
    auto exact = mcopt::BlackScholesAnalytical::calculate(
        S0, K, T, r, sigma, mcopt::OptionType::Call);

    std::cout << "\n[Analytical]" << std::endl;
    std::cout << "Price: " << exact.price << std::endl;
    std::cout << "Delta: " << exact.delta << std::endl;
    std::cout << "Gamma: " << exact.gamma << std::endl;

    // 2. Monte Carlo
    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma);
    
    unsigned long long paths = 5'000'000;
    std::cout << "\n[Monte Carlo (" << paths << " paths)] calculating..." << std::endl;

    auto mcResult = engine.calculateGreeks(paths);

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Price: " << mcResult.price << " (Diff: " << std::abs(mcResult.price - exact.price) << ")" << std::endl;
    std::cout << "Delta: " << mcResult.delta << " (Diff: " << std::abs(mcResult.delta - exact.delta) << ")" << std::endl;
    std::cout << "Gamma: " << mcResult.gamma << " (Diff: " << std::abs(mcResult.gamma - exact.gamma) << ")" << std::endl;

    return 0;
}