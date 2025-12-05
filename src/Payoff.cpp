#include "Payoff.hpp"

namespace mcopt {

double PayoffCall::operator()(double spot) const noexcept { return std::max(spot - m_strike, 0.0); }

double PayoffPut::operator()(double spot) const noexcept { return std::max(m_strike - spot, 0.0); }

}  // namespace mcopt