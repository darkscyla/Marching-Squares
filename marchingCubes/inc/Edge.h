#pragma once

#include "Node.h"

#include <array>

namespace marching_squares {

class Edge
{
public:
    Edge(const Node* origin, const Node* target);

    std::array<double, 3> GetIsoCoordinates(double iso_value) const;

private:
    const Node* origin_;
    const Node* target_;
};

} // namespace marching_squares
