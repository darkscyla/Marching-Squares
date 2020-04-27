#pragma once

#include <functional>

namespace cie {

class Node
{

private:
    const double val_;
    const double x_, y_;
    double z_ = 0;
    
public:
    Node(const double val, const double x, const double y);
    Node(const double val, const double x, const double y, const double z);

    double X() const;
    double Y() const;
    double Z() const;
    double Value() const;
};

} // namespace cie
