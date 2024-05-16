#ifndef ANTS_HPP
#define ANTS_HPP

#include "environment.hpp" //Pheromones, Food, Obstacles
#include "geometry.hpp"    //Vector2d
namespace kape {
class Ant
{
 private:
  Vector2d position_;
  Vector2d velocity_;
  bool has_food_;

  void calculateCirclesOfVision(std::array<Circle, 3> & circles_of_vision) const;

 public:
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
  // may throw invalid_argument if to_anthill_ph isn't of type
  // Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
  // Pheromones::Type::TO_FOOD
  void update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
              Anthill& anthill, Obstacles const& obstacles,
              double delta_t = 0.01);
};
class Ants
{};

} // namespace kape

#endif