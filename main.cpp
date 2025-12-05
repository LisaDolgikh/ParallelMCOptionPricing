#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "src/Analytical.hpp"
#include "src/MCEngine.hpp"
#include "src/Payoff.hpp"

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  --spot <value>      Spot price of asset (default: 100.0)\n"
              << "  --strike <value>    Strike price (default: 100.0)\n"
              << "  --r <value>         Risk-free interest rate (default: 0.05)\n"
              << "  --sigma <value>     Volatility (default: 0.2)\n"
              << "  --time <value>      Time to maturity in years (default: 1.0)\n"
              << "  --paths <value>     Number of MC simulations (default: 1'000'000)\n"
              << "  --help              Show this help message\n";
}

int main(int argc, char* argv[]) {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    unsigned long long paths = 1'000'000;

    // Парсинг аргументов
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }

        // Проверяем, есть ли следующее значение
        if (i + 1 < argc) {
            try {
                if (arg == "--spot")
                    S0 = std::stod(argv[++i]);
                else if (arg == "--strike")
                    K = std::stod(argv[++i]);
                else if (arg == "--r")
                    r = std::stod(argv[++i]);
                else if (arg == "--sigma")
                    sigma = std::stod(argv[++i]);
                else if (arg == "--time")
                    T = std::stod(argv[++i]);
                else if (arg == "--paths")
                    paths = std::stoull(argv[++i]);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing value for " << arg << ": " << e.what() << std::endl;
                return 1;
            }
        }
    }

    std::cout << "=== Parallel Monte Carlo Option Pricer ===" << std::endl;
    std::cout << "Parameters: S0=" << S0 << ", K=" << K << ", T=" << T << ", r=" << r
              << ", sigma=" << sigma << std::endl;
    std::cout << "Simulations: " << paths << std::endl;

    // 1. Аналитическое решение (для сравнения)
    auto exact =
        mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);

    std::cout << "\n[Analytical (Black-Scholes)]" << std::endl;
    std::cout << "Price: " << exact.price << std::endl;
    std::cout << "Delta: " << exact.delta << std::endl;
    std::cout << "Gamma: " << exact.gamma << std::endl;

    // 2. Monte Carlo
    auto payoff = std::make_shared<mcopt::PayoffCall>(K);

    // Используем seed по умолчанию
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma, 12345);

    std::cout << "\n[Monte Carlo] Calculating..." << std::endl;
    auto mcResult = engine.calculateGreeks(paths);

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Price: " << mcResult.price
              << " \t(Error: " << std::abs(mcResult.price - exact.price) << ")" << std::endl;
    std::cout << "Delta: " << mcResult.delta
              << " \t(Error: " << std::abs(mcResult.delta - exact.delta) << ")" << std::endl;
    std::cout << "Gamma: " << mcResult.gamma
              << " \t(Error: " << std::abs(mcResult.gamma - exact.gamma) << ")" << std::endl;

    return 0;
}