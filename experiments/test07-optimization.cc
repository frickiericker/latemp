#include <fstream>
#include <iostream>
#include <string>

#include <cassert>
#include <cmath>

#include <Eigen/Core>

#include "array_io.hpp"
#include "cost_functions.hpp"


using Index = Eigen::ArrayXXd::Index;

struct Mesh {
    Eigen::ArrayXXd       positions;
    Eigen::ArrayXXd const attractors;
};

Mesh loadMesh(std::string lat, std::string atr)
{
    auto latFile = std::ifstream{lat};
    if (!latFile) {
        throw std::runtime_error("cannot open file " + lat);
    }
    auto const positions = loadArrayXXd(latFile);

    auto atrFile = std::ifstream{atr};
    if (!atrFile) {
        throw std::runtime_error("cannot open file " + atr);
    }
    auto const attractors = loadArrayXXd(atrFile);

    if (positions.rows() != attractors.rows() ||
        positions.cols() != attractors.cols()) {
        throw std::runtime_error("dimension mismatch");
    }

    return Mesh{positions, attractors};
}

void resolveCollisions(Mesh& mesh, double epsilon = 0.01)
{
    auto& positions = mesh.positions;

    for (Index i = 0; i < positions.rows(); ++i) {
        for (Index j = 1; j < positions.cols(); ++j) {
            if (positions(i, j) == positions(i, j - 1)) {
                positions(i, j) += epsilon;
            }
        }
    }
}

class MeshOptimizer {
  public:
    explicit MeshOptimizer(Mesh& mesh);

    MeshOptimizer& setRepulsionWeight(double w);
    MeshOptimizer& setAttractionWeight(double w);
    MeshOptimizer& setSmoothingWeight(double w);

    void optimize(double bound);

  private:
    void computeGradients(Eigen::ArrayXXd& gradients) const;
    void computeRepulsions(Eigen::ArrayXXd& gradients) const;
    void computeAttractions(Eigen::ArrayXXd& gradients) const;
    void computeSmoothings(Eigen::ArrayXXd& gradients) const;

  private:
    double _repulsionWeight = 1;
    double _attractionWeight = 1;
    double _smoothingWeight = 1;
    Eigen::ArrayXXd _gradientsCache;
    Mesh& _mesh;
};

MeshOptimizer::MeshOptimizer(Mesh& mesh)
    : _gradientsCache{mesh.positions.rows(), mesh.positions.cols()}
    , _mesh{mesh}
{
}

inline
MeshOptimizer& MeshOptimizer::setRepulsionWeight(double w)
{
    _repulsionWeight = w;
    return *this;
}

inline
MeshOptimizer& MeshOptimizer::setAttractionWeight(double w)
{
    _attractionWeight = w;
    return *this;
}

inline
MeshOptimizer& MeshOptimizer::setSmoothingWeight(double w)
{
    _smoothingWeight = w;
    return *this;
}

void MeshOptimizer::optimize(double bound)
{
    computeGradients(_gradientsCache);
    auto const rate = bound / _gradientsCache.maxCoeff();
    _mesh.positions -= rate * _gradientsCache;
}

void MeshOptimizer::computeGradients(Eigen::ArrayXXd& gradients) const
{
    gradients.setZero();
    computeRepulsions(gradients);
    computeAttractions(gradients);
    computeSmoothings(gradients);
}

void MeshOptimizer::computeRepulsions(Eigen::ArrayXXd& gradients) const
{
    auto const cutoffSpacing = 1.0 / _mesh.positions.rows();

    for (Index i = 0; i < _mesh.positions.rows(); ++i) {
        for (Index j = 1; j < _mesh.positions.cols(); ++j) {
            auto const cost = HardcoreCost(
                _repulsionWeight,
                cutoffSpacing
            );
            auto const disp = _mesh.positions(i, j) - _mesh.positions(i, j - 1);
            auto const grad = cost.evaluateGradient(disp);
            gradients(i, j) += grad;
            gradients(i, j - 1) -= grad;
        }
    }
}

void MeshOptimizer::computeAttractions(Eigen::ArrayXXd& gradients) const
{
    for (Index i = 0; i < _mesh.positions.rows(); ++i) {
        for (Index j = 0; j < _mesh.positions.cols(); ++j) {
            auto const cost = ShiftedSpringCost(
                _attractionWeight,
                _mesh.attractors(i, j)
            );
            gradients(i, j) += cost.evaluateGradient(_mesh.positions(i, j));
        }
    }
}

void MeshOptimizer::computeSmoothings(Eigen::ArrayXXd& gradients) const
{
    for (Index i = 1; i < _mesh.positions.rows(); ++i) {
        for (Index j = 0; j < _mesh.positions.cols(); ++j) {
            auto const cost = SpringCost(_smoothingWeight);
            auto const disp = _mesh.positions(i, j) - _mesh.positions(i - 1, j);
            auto const grad = cost.evaluateGradient(disp);
            gradients(i, j) += grad;
            gradients(i - 1, j) -= grad;
        }
    }
}

int main()
{
    auto mesh = loadMesh("test05-lat.tsv", "test09-lat.tsv");
    resolveCollisions(mesh);

    auto opt = MeshOptimizer{mesh};
    opt.setRepulsionWeight(1.0)
       .setAttractionWeight(1.0)
       .setSmoothingWeight(5.0);

    for (int step = 0; step < 100000; ++step) {
        opt.optimize(0.001);
    }

    for (Index j = 0; j < mesh.positions.cols(); ++j) {
        for (Index i = 1; i < mesh.positions.rows(); ++i) {
            if (mesh.positions(i, j) > mesh.positions(i - 1, j)) {
                std::cerr << "Anomaly at " << i << ',' << j << '\n';
            }
        }
    }

    std::cout << mesh.positions << '\n';
}
