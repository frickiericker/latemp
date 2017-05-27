#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <cassert>
#include <cmath>

#include <Eigen/Core>


class System {
  public:
    System();

    void quench();
    double step();
    void print() const;

  private:
    double compute_repulsion();
    double compute_attraction();

  private:
    int _num_points;
    double _deformation_rate;
    double _reference_spacing;
    double _repulsion_weight;
    Eigen::ArrayXd _positions;
    Eigen::ArrayXd _attractors;
    Eigen::ArrayXd _gradients;
};

System::System()
{
    _num_points = 73;
    _deformation_rate = 0.01;
    _reference_spacing = 1.0 / _num_points;
    _repulsion_weight = 0.01;
    _positions = Eigen::ArrayXd(_num_points, 1);
    _attractors = Eigen::ArrayXd(_num_points, 1);
    _gradients = Eigen::ArrayXd(_num_points, 1);

    _positions <<
        1, 0.972222, 0.944444, 0.916667, 0.888889, 0.861111, 0.833333,
        0.805556, 0.777778, 0.75, 0.722222, 0.694444, 0.666667, 0.638889,
        0.611111, 0.583333, 0.555556, 0.527778, 0.5, 0.472222, 0.444444,
        0.416667, 0.388889, 0.361111, 0.333333, 0.305556, 0.277778, 0.25,
        0.222222, 0.194444, 0.166667, 0.138889, 0.111111, 0.0833333,
        0.0555556, 0.0277778, 0, -0.0277778, -0.0555556, -0.0833333,
        -0.111111, -0.138889, -0.166667, -0.194444, -0.222222, -0.25,
        -0.277778, -0.305556, -0.333333, -0.361111, -0.388889, -0.416667,
        -0.444444, -0.472222, -0.5, -0.527778, -0.555556, -0.583333,
        -0.611111, -0.638889, -0.666667, -0.694444, -0.722222, -0.75,
        -0.777778, -0.805556, -0.833333, -0.861111, -0.888889, -0.916667,
        -0.944444, -0.972222, -1;
    _attractors <<
        0.772878, 0.774021, 0.771908, 0.764386, 0.766723, 0.753051, 0.75021,
        0.74223, 0.72968, 0.723665, 0.688848, 0.66151, 0.634354, 0.604848,
        0.591548, 0.578627, 0.56382, 0.538319, 0.51461, 0.476298, 0.434346,
        0.400212, 0.35973, 0.326613, 0.280709, 0.244313, 0.217969, 0.190213,
        0.175073, 0.161105, 0.152211, 0.132007, 0.115429, 0.081012, 0.0483095,
        0.0567881, 0.0310131, 0.0216047, 0.0155065, 0.032133, 0.055017,
        0.0452871, 0.0854309, 0.148781, 0.195964, 0.246733, 0.287185,
        0.320824, 0.352863, 0.377065, 0.40187, 0.416441, 0.436117, 0.461349,
        0.48157, 0.500297, 0.513716, 0.530605, 0.546404, 0.56596, 0.592468,
        0.621685, 0.668201, 0.729292, 0.765874, 0.780463, 0.805708, 0.833153,
        0.83457, 0.801369, 0.851291, 0.962501, 0.999441;

    assert(_positions.size() == _attractors.size());
    assert(_positions.size() == _gradients.size());
}

void System::quench()
{
    _deformation_rate /= 2;
}

double System::step()
{
    _gradients.setZero();
    auto const r_cost = compute_repulsion();
    auto const a_cost = compute_attraction();
    _positions -= _deformation_rate * _gradients;
    return r_cost + a_cost;
}

double System::compute_repulsion()
{
    double cost = 0;
    auto const norm = 1 / (_reference_spacing * _reference_spacing);
    for (int i = 0; i + 1 < _num_points; ++i) {
        auto const spacing = _positions[i] - _positions[i + 1];
        auto const jam = 1 - norm * spacing * spacing;
        if (jam < 0) {
            continue;
        }
        cost += _repulsion_weight * (jam * jam * jam);
        auto const grad = -3 * _repulsion_weight * (jam * jam) * norm * spacing;
        _gradients[i] += grad;
        _gradients[i + 1] -= grad;
    }
    return cost;
}

double System::compute_attraction()
{
    double cost = 0;
    for (int i = 0; i < _num_points; ++i) {
        auto const dir = std::copysign(1.0, _positions[i]);
        auto const dist = std::fabs(_positions[i]) - _attractors[i];
        auto const grad = 2 * dir * dist;
        cost += dist * dist;
        _gradients[i] += grad;
    }
    return cost;
}

void System::print() const
{
    std::cout << _positions << '\n';
}

int main()
{
    auto system = System();

    for (int step = 0; step < 30000; ++step) {
        if (step % 3000 == 2999) {
            system.quench();
        }
        system.step();
    }

    system.print();
}
