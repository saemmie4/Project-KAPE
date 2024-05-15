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

 public:
  static double const ANT_LENGTH{0.005}; //0.5 cm
  static double const ANT_SPEED{0.05}; //5 cm/s
  static double const CIRCLE_OF_VISION_RADIUS{ANT_LENGTH}; 
  static double const CIRCLE_OF_VISION_DISTANCE{ANT_LENGTH}; 
  static double const CIRCLE_OF_VISION_ANGLE{PI/3.}; 



  explicit Ant(Vector2d const& position, Vector2d const& velocity,
               bool has_food = false);

  Vector2d getPosition() const;
  bool hasFood() const;
  void update( Pheromones& to_anthill_ph,
              Pheromones& to_food_ph, Food& food,
              Obstacles const& obstacles, double delta_t= 0.01);
};
class Ants
{};

} // namespace kape

#endif