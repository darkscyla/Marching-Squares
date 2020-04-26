#include "Node.h"

namespace cie {

Node::Node(const Function2D& function, const double x, const double y) :
    x_(x), y_(y), val_(function(x, y))
{
}

Node::Node(const Function3D& function, const double x, const double y, const double z):
    x_(x), y_(y), z_(z), val_(function(x, y, z))
{
}

double Node::X() const
{
    return x_;
}

double Node::Y() const
{
    return y_;
}

double Node::Z() const
{
    return z_;
}

double Node::Value() const
{
    return val_;
}
} // namespace cie
