#pragma once

#include "Node.h"

#include <array>

namespace cie {

class Edge
{
private:
	const Node origin_, target_;

public:
	Edge(Node origin, Node target);

	std::array<double, 3> GetIsoCoordinates(const double iso_value) const;
};
} // namespace cie
