#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "src/Analytical.hpp"
#include "src/MCEngine.hpp"
#include "src/Payoff.hpp"
#include "src/ResultsExporter.hpp"
namespace fs = std::filesystem;

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  --spot <value>      Spot price of asset (default: 100.0)\n"
              << "  --strike <value>    Strike price (default: 100.0)\n"
              << "  --r <value>         Risk-free interest rate (default: 0.05)\n"
              << "  --sigma <value>     Volatility (default: 0.2)\n"
              << "  --time <value>      Time to maturity in years (default: 1.0)\n"
              << "  --paths <value>     Number of MC simulations (default: 1'000'000)\n"
              << "  --steps <value>     Steps for Asian Option (default: 252)\n"
              << "  --help              Show this help message\n";
}

int main(int argc, char* argv[]) {
    double S0 = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    unsigned long long paths = 1'000'000;
    unsigned int steps = 252;

    // Парсинг аргументов
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }

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
                else if (arg == "--steps")
                    steps = std::stoul(argv[++i]);  // Новый параметр
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
    std::cout << "Asian Steps: " << steps << " (Time discretization)" << std::endl;

    // ==========================================
    // 1. Analytical Solution (Reference)
    // ==========================================
    auto exact =
        mcopt::BlackScholesAnalytical::calculate(S0, K, T, r, sigma, mcopt::OptionType::Call);

    std::cout << "\n[1. Analytical (European Call)]" << std::endl;
    std::cout << std::fixed << std::setprecision(5);

    std::cout << std::left << std::setw(8) << "Price:" << std::setw(12) << exact.price << std::endl;

    std::cout << std::left << std::setw(8) << "Delta:" << std::setw(12) << exact.delta << std::endl;

    std::cout << std::left << std::setw(8) << "Gamma:" << std::setw(12) << exact.gamma << std::endl;

    // ==========================================
    // 2. Monte Carlo (European)
    // ==========================================

    auto payoffEur = std::make_shared<mcopt::PayoffCall>(K);
    mcopt::MonteCarloEngine engineEur(payoffEur, S0, T, r, sigma, 12345);  // Seed 12345

    std::cout << "\n[2. Monte Carlo (European Call)]" << std::endl;

    auto startEur = std::chrono::high_resolution_clock::now();
    auto mcResult = engineEur.calculateGreeks(paths);
    auto endEur = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffEur = endEur - startEur;

    std::cout << std::fixed << std::setprecision(5);

    std::cout << std::left << std::setw(8) << "Price:" << std::setw(12) << mcResult.price
              << "(Error: " << std::abs(mcResult.price - exact.price) << ")" << std::endl;

    std::cout << std::left << std::setw(8) << "Delta:" << std::setw(12) << mcResult.delta
              << "(Error: " << std::abs(mcResult.delta - exact.delta) << ")" << std::endl;

    std::cout << std::left << std::setw(8) << "Gamma:" << std::setw(12) << mcResult.gamma
              << "(Error: " << std::abs(mcResult.gamma - exact.gamma) << ")" << std::endl;

    fs::path outputDir = fs::path("..") / "out";
    // Создаем папку out, если её еще нет if (!fs::exists(outputDir))
    {
        fs::create_directories(outputDir);
    }

    // Полный путь к файлу: ../out/pricing_results.csv
    std::string csvPath = (outputDir / "pricing_results.csv").string();

    mcopt::ResultsExporter::exportToCSV(csvPath, "European Call", S0, K, T, r, sigma, paths,
                                        0,  // Steps = 0 для Европейского
                                        mcResult.price, mcResult.delta, mcResult.gamma,
                                        diffEur.count());

    // ==========================================
    // 3. Monte Carlo (Asian - Path Dependent)
    // ==========================================
    auto payoffAsian = std::make_shared<mcopt::PayoffAsianCall>(K);
    mcopt::MonteCarloEngine engineAsian(payoffAsian, S0, T, r, sigma, 12345);

    std::cout << "\n[3. Monte Carlo (Asian Arithmetic Call)]" << std::endl;

    auto startAsian = std::chrono::high_resolution_clock::now();
    double priceAsian = engineAsian.calculateAsianPrice(paths, steps);
    auto endAsian = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffAsian = endAsian - startAsian;

    std::cout << std::left << std::setw(8) << "Price:" << std::setw(12) << priceAsian << std::endl;

    std::cout << std::left << std::setw(8) << "Time:" << std::setw(12) << std::setprecision(4)
              << diffAsian.count() << " sec" << std::endl;

    std::cout << std::setprecision(5);
    std::cout << "Note: Asian Price (" << priceAsian << ") < European Price (" << mcResult.price
              << ") due to volatility averaging effect." << std::endl;

    mcopt::ResultsExporter::exportToCSV(csvPath, "Asian Call", S0, K, T, r, sigma, paths,
                                        steps,                 // Важно: сохраняем количество шагов
                                        priceAsian, 0.0, 0.0,  // Delta/Gamma не считались
                                        diffAsian.count());

    return 0;
}