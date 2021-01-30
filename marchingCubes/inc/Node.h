#pragma once

namespace marching_squares {

class Node
{
public:
    Node(double val, double x, double y);
    Node(double val, double x, double y, double z);

    double X() const;
    double Y() const;
    double Z() const;
    double Value() const;
private:
    const double val_;
    const double x_, y_;
    double z_ = 0;
};

} // namespace marching_squares
