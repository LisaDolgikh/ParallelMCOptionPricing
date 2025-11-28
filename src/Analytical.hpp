#pragma once

namespace mcopt {

    enum class OptionType { Call, Put };

    struct Greeks {
        double price;
        double delta;
        double gamma;
    };

    class BlackScholesAnalytical {
    public:
        [[nodiscard]] static Greeks calculate(
            double S, double K, double T, double r, double sigma, OptionType type);
    };

} // namespace mcopt