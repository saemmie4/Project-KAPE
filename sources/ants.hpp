#ifndef ANTS_HPP
#define ANTS_HPP

#include "geometry.hpp" //Vector2d
namespace kape {

class Ant
{
 private:
  Vector2d position_;
  Vector2d velocity_;
  bool has_food_;

public:
explicit Ant(Vector2d const& position, Vector2d const& velocity, bool has_food = false);

Vector2d getPosition() const;
bool hasFood() const;
};

} // namespace kape

#endif