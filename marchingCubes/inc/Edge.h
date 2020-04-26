#pragma once

#include "Node.h"

#include <array>

namespace cie {

class Edge
{
private:
    const Node* origin_;
    const Node* target_;

public:
    Edge(const Node* origin, const Node* target);

    std::array<double, 3> GetIsoCoordinates(const double iso_value) const;
};
} // namespace cie
