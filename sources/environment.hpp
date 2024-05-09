#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "geometry.hpp" //for Vector2d

// TODO:
//  everything

namespace kape {
class FoodParticle
{
 private:
  Vector2d position_;

 public:
  FoodParticle(Vector2d const& position);
};

class Pheromone
{
 private:
  Vector2d position_;
  int intensity_;

 public:
  Pheromone(Vector2d const& position, int intensity_ = 100);
  Vector2d getPosition() const;
  //returns the intensity as a number in the interval [0, 100]
  int getIntensity() const;
  
  void decreaseIntensity(int amount = 5);
  //returns true if the Pheromone's intensity is 0
  bool hasEvaporated() const;
};

class Environment
{};
} // namespace kape

#endif