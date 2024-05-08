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

class Environment
{};
} // namespace kape

#endif