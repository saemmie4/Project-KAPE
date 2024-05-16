#include "ants.hpp"
#include "environment.hpp"
#include <array> //for circles of vision of the ant
#include <cmath>
#include <stdexcept> //invalid_argument
namespace kape {

// TODO:
//-improve the ant's update method implementation

void Ant::calculateCirclesOfVision(
    std::array<Circle, 3>& circles_of_vision) const
{
  // note: velocity can't be null for class invariant
  Vector2d facing_dir{velocity_ / norm(velocity_)};

  double angle{-CIRCLE_OF_VISION_ANGLE};
  for (auto& cov : circles_of_vision) {
    cov.setCircleRadius(CIRCLE_OF_VISION_RADIUS);
    cov.setCircleCenter(
        position_ + CIRCLE_OF_VISION_DISTANCE * rotate(facing_dir, angle));
    angle += CIRCLE_OF_VISION_ANGLE;
  }
}

// may throw std::invalid_argument if velocity is null
Ant::Ant(Vector2d const& position, Vector2d const& velocity, bool has_food)
    : position_{position}
    , velocity_{velocity}
    , has_food_{has_food}
{
  if (norm2(velocity) == 0.) {
    throw std::invalid_argument{"the ant's velocity can't be null"};
  }
}

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

// may throw invalid_argument if to_anthill_ph isn't of type
// Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
// Pheromones::Type::TO_FOOD
void Ant::update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
                 Anthill& anthill, Obstacles const& obstacles, double delta_t)
{
  if (to_anthill_ph.getPheromonesType() != Pheromones::Type::TO_ANTHILL) {
    throw std::invalid_argument{
        "The parameter to_anthill_ph, passed to Ant::update(), isn't of type "
        "Pheromones::Type::TO_ANTHILL"};
  }
  if (to_food_ph.getPheromonesType() != Pheromones::Type::TO_FOOD) {
    throw std::invalid_argument{
        "The parameter to_food_ph, passed to Ant::update(), isn't of type "
        "Pheromones::Type::TO_FOOD"};
  }

  position_ += delta_t * velocity_;

  //[0]: left [1]: center [2]: right
  std::array<Circle, 3> circles_of_vision;
  calculateCirclesOfVision(circles_of_vision);

  if (has_food_) {                     // has food
    if (anthill.isInside(position_)) { // inside anthill
      anthill.addFood();
      has_food_ = false;
    } else { // outside the anthill
      to_food_ph.addPheromoneParticle(position_);
    }
  } else { // has no food
    to_anthill_ph.addPheromoneParticle(position_);
    for (auto const& cov : circles_of_vision) {
      if (food.removeOneFoodParticleInCircle(cov)) {
        has_food_ = true;
        velocity_ *= -1.;
        calculateCirclesOfVision(circles_of_vision);
        break;
      }
    }
  }
}

} // namespace kape
