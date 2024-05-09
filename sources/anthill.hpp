#ifndef COLONY_HPP
#define COLONY_HPP

#include "geometry.hpp" //for vector2d

namespace kape {
class Colony
{
 private:
  Vector2d position_;
  double radius_;
  int food_counter_;

 public:
  // may throw std::invalid_argument if radius<=0 or food_counter < 0
  explicit Colony(Vector2d position, double radius, int food_counter = 0);
  // may throw std::invalid_argument if amount < 0
  void addFood(int amount = 1);

  Vector2d getPosition() const;
  double getRadius() const;
  int getFoodCounter() const;
};
} // namespace kape

#endif