#include "Node.h"

namespace marching_squares {

Node::Node(const double val, const double x, const double y) :
    val_(val), x_(x), y_(y)
{
}

Node::Node(const double val, const double x, const double y, const double z):
    val_(val), x_(x), y_(y), z_(z)
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
} // namespace marching_squares
