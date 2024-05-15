#include "ants.hpp"
#include <array>

namespace kape {
Ant::Ant(Vector2d const& position, Vector2d const& velocity, bool has_food)
    : position_{position}
    , velocity_{velocity}
    , has_food_{has_food}
{}

Vector2d Ant::getPosition() const
{
  return position_;
}
bool Ant::hasFood() const
{
  return has_food_;
}

void Ant::update(Pheromones& to_anthill_ph, Pheromones& to_food_ph, Food& food,
                 Obstacles const& obstacles, double delta_t)
{
  position_ += delta_t * velocity_;

  std::Arra
}

} // namespace kape
