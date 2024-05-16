#include "ants.hpp"
#include "environment.hpp"
#include <array> //for circles of vision of the ant
#include <cmath>
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

double Ant::getFacingAngle() const
{
  if (velocity_.x == 0. && velocity_.y == 0.) {
    return 0.;
  }

  // computes the angle in [-pi, +pi]
  return atan2(velocity_.y, velocity_.x);
}

bool Ant::hasFood() const
{
  return has_food_;
}

void Ant::update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
                 Anthill& anthill, Obstacles const& obstacles, double delta_t)
{
  position_ += delta_t * velocity_;

  if (has_food_) {
    to_food_ph.addPheromoneParticle(position_);
  } else {
    to_anthill_ph.addPheromoneParticle(position_);
  }

  // Vector2d facing_dir{velocity_ / norm(velocity_)};
  // std::array<Circle, 3> circles_of_vision;
}

} // namespace kape
