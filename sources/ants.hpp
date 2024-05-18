#ifndef ANTS_HPP
#define ANTS_HPP

#include "environment.hpp" //Pheromones, Food, Obstacles
#include "geometry.hpp"    //Vector2d
#include <array>
#include <random>
namespace kape {
class Ant
{
 private:
  // every PERIOD_BETWEEN_PHEROMONE_RELEASE_ the ant releases a pheromone
  static double constexpr PERIOD_BETWEEN_PHEROMONE_RELEASE_{.1};

  Vector2d position_;
  Vector2d velocity_;
  bool has_food_;
  double time_since_last_pheromone_release_;

  double calculateAngleToAvoidObstacles(
      std::array<Circle, 3> const& cov, Obstacles obs,
      std::default_random_engine& random_engine) const;

  double calculateAngleFromPheromones(std::array<Circle, 3> const& cov,
                                      Pheromones const& ph_to_follow) const;
  double
  calculateRandomTurning(std::default_random_engine& random_engine) const;

 public:
  void calculateCirclesOfVision(std::array<Circle, 3>& circles_of_vision) const;
  static double constexpr ANT_LENGTH{0.005}; // 0.5 cm
  static double constexpr ANT_SPEED{0.05};   // 5 cm/s
  static double constexpr CIRCLE_OF_VISION_RADIUS{ANT_LENGTH};
  static double constexpr CIRCLE_OF_VISION_DISTANCE{1.5 * ANT_LENGTH};
  static double constexpr CIRCLE_OF_VISION_ANGLE{PI / 3.};

  // may throw std::invalid_argument if velocity is null
  explicit Ant(Vector2d const& position, Vector2d const& velocity,
               bool has_food = false);

  Vector2d getPosition() const;
  // if velocity == {0.,0.} instead of the angle it returns 0.
  double getFacingAngle() const;
  bool hasFood() const;
  // may throw std::invalid_argument if to_anthill_ph isn't of type
  // Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
  // Pheromones::Type::TO_FOOD
  // may throw std::invalid_argument if delta_t < 0.
  void update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
              Anthill& anthill, Obstacles const& obstacles,
              std::default_random_engine& random_engine, double delta_t = 0.01);
};
class Ants
{};

} // namespace kape

#endif