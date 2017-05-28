#ifndef COST_FUNCTIONS_HPP
#define COST_FUNCTIONS_HPP

#include <algorithm>

#include "arithmetic_math.hpp"

/*
 * SoftcoreCost is a cost function that penaltizes collision of two balls.
 */
class SoftcoreCost {
  public:
    explicit SoftcoreCost(double weight, double distance);
    double evaluateCost(double displace) const;
    double evaluateGradient(double displace) const;

  private:
    double computeOverlap(double displace) const;

  private:
    double _weight = 1;
    double _normalizer = 1;
};

inline
SoftcoreCost::SoftcoreCost(double weight, double distance)
    : _weight{weight}
    , _normalizer{1 / (distance * distance)}
{
}

inline
double SoftcoreCost::evaluateCost(double displace) const
{
    return _weight * math::pow<3>(computeOverlap(displace));
}

inline
double SoftcoreCost::evaluateGradient(double displace) const
{
    return -6 * _weight * _normalizer * math::pow<2>(computeOverlap(displace)) * displace;
}

inline
double SoftcoreCost::computeOverlap(double displace) const
{
    return std::max(0.0, 1.0 - _normalizer * displace * displace);
}

/*
 *
 */
class HardcoreCost {
  public:
    HardcoreCost(double weight, double cutoff);
    double evaluateCost(double distance) const;
    double evaluateGradient(double distance) const;

  private:
    double _weight = 1;
    double _cutoff = 1;
};

HardcoreCost::HardcoreCost(double weight, double cutoff)
    : _weight{weight}
    , _cutoff{cutoff}
{
}

double HardcoreCost::evaluateCost(double distance) const
{
    auto const u = std::max(1.0, _cutoff / distance);
    auto const u3 = u * u * u;
    auto const u6 = u3 * u3;
    return _weight * ((u6 - 2.0) * u6 + 1.0);
}

double HardcoreCost::evaluateGradient(double distance) const
{
    auto const u = std::max(1.0, _cutoff / distance);
    auto const u3 = u * u * u;
    auto const u6 = u3 * u3;
    auto const u12 = u6 * u6;
    return -12 * _weight * (u12 - u6) / distance;
}

/*
 * SpringCost is a cost function that penaltizes dispersion of two points.
 */
class SpringCost {
  public:
    explicit SpringCost(double weight);
    double evaluateCost(double displace) const;
    double evaluateGradient(double displace) const;

  private:
    double _weight = 1;
};

inline
SpringCost::SpringCost(double weight)
    : _weight{weight}
{
}

inline
double SpringCost::evaluateCost(double displace) const
{
    return _weight * math::pow<2>(displace);
}

inline
double SpringCost::evaluateGradient(double displace) const
{
    return 2 * _weight * displace;
}

/*
 * ShiftedSpringCost is a cost function that penaltizes dispersion of two points
 * from defined distance.
 */
class ShiftedSpringCost {
  public:
    ShiftedSpringCost(double weight, double shift);
    double evaluateCost(double displace) const;
    double evaluateGradient(double displace) const;

  private:
    double computeDispersion(double displace) const;

  private:
    double _weight = 1;
    double _shift = 0;
};

inline
ShiftedSpringCost::ShiftedSpringCost(double weight, double shift)
    : _weight{weight}
    , _shift{shift}
{
}

inline
double ShiftedSpringCost::evaluateCost(double displace) const
{
    return _weight * math::pow<2>(computeDispersion(displace));
}

inline
double ShiftedSpringCost::evaluateGradient(double displace) const
{
    return 2 * _weight * math::signum(displace) * computeDispersion(displace);
}

inline
double ShiftedSpringCost::computeDispersion(double displace) const
{
    return math::abs(displace) - _shift;
}

#endif
