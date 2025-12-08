#pragma once

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace mcopt {

/**
 * @class ResultsExporter
 * @brief Класс для сохранения результатов расчетов в CSV.
 * Поддерживает Европейские и Азиатские опционы.
 */
class ResultsExporter {
   public:
    /**
     * @brief Универсальный метод экспорта в CSV.
     * * @param filename Имя файла.
     * @param type Тип опциона (напр. "European Call", "Asian Call").
     * @param S0, K, T, r, sigma Параметры рынка.
     * @param paths Число путей Монте-Карло.
     * @param steps Число шагов по времени (0 для Европейских).
     * @param price Рассчитанная цена.
     * @param delta, gamma Греки (передавать 0.0, если не рассчитывались).
     * @param timeElapsed Время выполнения в секундах.
     */
    static void exportToCSV(const std::string& filename, const std::string& type, double S0,
                            double K, double T, double r, double sigma, unsigned long long paths,
                            unsigned int steps, double price, double delta, double gamma,
                            double timeSec) {
        bool fileExists = std::filesystem::exists(filename);

        // Открываем файл на дозапись
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "[Error] Could not open file " << filename << " for writing." << std::endl;
            return;
        }

        // Если файл новый, пишем заголовок
        if (!fileExists) {
            file << "Type,Spot,Strike,Time,Rate,Sigma,Steps,Paths,Price,Delta,Gamma,Time_Sec\n";
        }

        // Пишем данные с фиксированной точностью
        file << type << "," << S0 << "," << K << "," << T << "," << r << "," << sigma << ","
             << steps << "," << paths << "," << std::fixed << std::setprecision(5) << price << ","
             << delta << "," << gamma << "," << std::setprecision(6) << timeSec << "\n";

        std::cout << "[Info] Result saved to " << filename << std::endl;
    }
};

}  // namespace mcopt