#ifndef ARRAY_IO_HPP
#define ARRAY_IO_HPP

#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <Eigen/Core>

inline
Eigen::ArrayXXd loadArrayXXd(std::istream& in)
{
    using Index = Eigen::ArrayXXd::Index;

    std::vector<double> values;
    Index rows = 0;
    Index items = 0;
    for (std::string line; std::getline(in, line); ) {
        auto lineStream = std::istringstream{line};
        for (double val; lineStream >> val; ) {
            values.push_back(val);
            ++items;
        }
        if (!lineStream.eof()) {
            throw std::runtime_error{"failed to parse input"};
        }
        ++rows;
    }
    if (!in.eof()) {
        throw std::runtime_error{"failed to load from input"};
    }
    if (items == 0) {
        throw std::runtime_error{"input is empty"};
    }
    if (items % rows != 0) {
        throw std::runtime_error{"input is not rectangular"};
    }
    auto const cols = items / rows;

    auto array = Eigen::ArrayXXd(rows, cols);
    for (Index i = 0; i < rows; ++i) {
        for (Index j = 0; j < cols; ++j) {
            array(i, j) = values[i * cols + j];
        }
    }
    return array;
}

#endif
