#pragma once

#include <functional>

namespace cie {

class Node
{
using Function2D = std::function<double(double, double)>;
using Function3D = std::function<double(double, double, double)>;

private:
    const double x_, y_;
    double z_ = 0;
    const double val_;
    
public:
    Node(const Function2D& function, const double x, const double y);
    Node(const Function3D& function, const double x, const double y, const double z);

    double X() const;
    double Y() const;
    double Z() const;
    double Value() const;
};

} // namespace cie
