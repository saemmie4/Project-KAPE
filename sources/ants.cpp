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

  double angle{CIRCLE_OF_VISION_ANGLE};
  for (auto& cov : circles_of_vision) {
    cov.setCircleRadius(CIRCLE_OF_VISION_RADIUS);
    cov.setCircleCenter(
        position_ + CIRCLE_OF_VISION_DISTANCE * rotate(facing_dir, angle));
    angle -= CIRCLE_OF_VISION_ANGLE;
  }
}

// may throw std::invalid_argument if velocity is null
Ant::Ant(Vector2d const& position, Vector2d const& velocity, bool has_food)
    : position_{position}
    , velocity_{velocity}
    , has_food_{has_food}
    , time_since_last_pheromone_release_{0.}
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

double Ant::calculateAngleToAvoidObstacles(
    std::array<Circle, 3> const& cov, Obstacles obs,
    std::default_random_engine& random_engine) const
{
  double const LEFT_RIGHT_ANGLE{PI / 6.};
  double const AHEAD_ANGLE{PI / 2.};
  double const AHED_ANGLE_MULTIPLIER{4.};

  double rotate_by_angle{0.};

  bool any_obs_left{obs.anyObstaclesInCircle(cov[0])};
  bool any_obs_ahead{obs.anyObstaclesInCircle(cov[1])};
  bool any_obs_right{obs.anyObstaclesInCircle(cov[2])};

  if (!any_obs_left && any_obs_ahead && !any_obs_right) {
    std::uniform_int_distribution coinflip{0, 1};
    // pick at random left or right
    return AHEAD_ANGLE * (coinflip(random_engine) == 0 ? -1 : 1);
  }

  if (any_obs_left) {
    rotate_by_angle -= LEFT_RIGHT_ANGLE;
  }
  if (any_obs_right) {
    rotate_by_angle += LEFT_RIGHT_ANGLE;
  }
  if (any_obs_ahead) {
    rotate_by_angle = 2 * AHEAD_ANGLE - AHED_ANGLE_MULTIPLIER * rotate_by_angle;
  }

  return rotate_by_angle;
}

double Ant::calculateAngleFromPheromones(std::array<Circle, 3> const& cov,
                                         Pheromones const& ph_to_follow) const
{
  double rotate_by_angle{0.};

  return rotate_by_angle;
}
double
Ant::calculateRandomTurning(std::default_random_engine& random_engine) const
{
  double rotate_by_angle{0.};

  return rotate_by_angle;

}

// may throw invalid_argument if to_anthill_ph isn't of type
// Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
// Pheromones::Type::TO_FOOD
void Ant::update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
                 Anthill& anthill, Obstacles const& obstacles,
                 std::default_random_engine& random_engine, double delta_t)
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
  if (delta_t < 0.) {
    throw std::invalid_argument{"delta_t can't be negative"};
  }

  time_since_last_pheromone_release_ += delta_t;
  bool time_to_release_pheromone{false};
  if (time_since_last_pheromone_release_ > PERIOD_BETWEEN_PHEROMONE_RELEASE_) {
    time_since_last_pheromone_release_ -= PERIOD_BETWEEN_PHEROMONE_RELEASE_;
    time_to_release_pheromone = true;
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
      if (time_to_release_pheromone) {
        to_food_ph.addPheromoneParticle(position_);
      }
    }
  } else { // has no food
    if (time_to_release_pheromone) {
      to_anthill_ph.addPheromoneParticle(position_);
    }
    for (auto const& cov : circles_of_vision) {
      if (food.removeOneFoodParticleInCircle(cov)) {
        has_food_ = true;
        velocity_ *= -1.;
        calculateCirclesOfVision(circles_of_vision);
        break;
      }
    }
  }

  // avoid obstacles
  double angle_to_avoid_obstacles{calculateAngleToAvoidObstacles(
      circles_of_vision, obstacles, random_engine)};

  if (angle_to_avoid_obstacles != 0.) {
    velocity_ = rotate(velocity_, angle_to_avoid_obstacles);
    calculateCirclesOfVision(circles_of_vision);
  }

  // follow appropriate pheromone + randomness
  Pheromones& pheromone_to_follow{has_food_ ? to_anthill_ph : to_food_ph};
  double angle_chosen{
      calculateAngleFromPheromones(circles_of_vision, pheromone_to_follow)};
  angle_chosen += calculateRandomTurning(random_engine);

  velocity_ = rotate(velocity_, angle_chosen);
}

} // namespace kape
