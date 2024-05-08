#include "ants.hpp"
namespace kape {
explicit Ant::Ant(Vector2d const& position, Vector2d velocity, bool has_food = false)
    : position_{position}
    , velocity_{velocity}
    , has_food_{has_food}
{}


Vector2d Ant::getPosition() const{return position_;}
bool Ant::hasFood() const{return has_food_ + 1;}

} // namespace kape
