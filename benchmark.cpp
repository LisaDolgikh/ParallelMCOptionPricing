#include <chrono>   // Для замеров времени
#include <iomanip>  // Для красивого вывода (setw)
#include <iostream>
#include <memory>
#include <vector>

#include "src/MCEngine.hpp"
#include "src/Payoff.hpp"

// Константы для теста
const double S0 = 100.0;
const double K = 100.0;
const double T = 1.0;
const double r = 0.05;
const double sigma = 0.2;
const unsigned long long NUM_PATHS = 10'000'000;

int main() {
    std::cout << "=== Monte Carlo Performance Benchmark ===" << std::endl;
    std::cout << "Paths: " << NUM_PATHS << std::endl;

    // Подготовка движка
    auto payoff = std::make_shared<mcopt::PayoffCall>(K);
    // Используем фиксированный seed, чтобы вычисления были идентичны
    mcopt::MonteCarloEngine engine(payoff, S0, T, r, sigma, 12345);

    // Определяем доступное число потоков
    unsigned int maxThreads = std::thread::hardware_concurrency();
    if (maxThreads == 0) maxThreads = 4;  // Fallback

    std::cout << "Hardware Concurrency: " << maxThreads << " threads\n" << std::endl;

    // Заголовок таблицы
    std::cout << std::left << std::setw(10) << "Threads" << std::setw(15) << "Time (sec)"
              << std::setw(15) << "Price" << std::setw(10) << "Speedup" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    double baseTime = 0.0;  // Время на 1 потоке

    // Цикл по количеству потоков
    for (unsigned int t = 1; t <= maxThreads; ++t) {
        // 1. Устанавливаем число потоков
        engine.setNumThreads(t);

        // 2. Замеряем время
        auto start = std::chrono::high_resolution_clock::now();

        double price = engine.calculatePrice(NUM_PATHS);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        double timeSec = diff.count();

        // 3. Считаем ускорение (Speedup)
        if (t == 1) {
            baseTime = timeSec;
        }
        double speedup = baseTime / timeSec;

        // 4. Вывод строки таблицы
        std::cout << std::left << std::setw(10) << t << std::setw(15) << std::fixed
                  << std::setprecision(4) << timeSec << std::setw(15) << std::fixed
                  << std::setprecision(5) << price << std::setw(10) << std::fixed
                  << std::setprecision(2) << speedup << "x" << std::endl;
    }

    std::cout << "\nBenchmark finished." << std::endl;
    return 0;
}
