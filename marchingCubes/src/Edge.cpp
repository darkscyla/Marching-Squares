#include "Edge.h"

namespace marching_squares {

Edge::Edge(const Node* origin, const Node* target):
    origin_(origin), target_(target)
{
}

std::array<double, 3> Edge::GetIsoCoordinates(const double iso_value) const
{

    const auto t = (iso_value - origin_->Value()) / (target_->Value() - origin_->Value());

    return {
        (1 - t) * origin_->X() + t * target_->X(),
        (1 - t) * origin_->Y() + t * target_->Y(),
        (1 - t) * origin_->Z() + t * target_->Z()
    };
}

} // namespace marching_squares
