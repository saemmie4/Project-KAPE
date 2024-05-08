#include "geometry.hpp"
#include <cmath>

namespace kape {

Vector2d& Vector2d::operator+=(Vector2d const& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

// dot product
double operator*(Vector2d const& lhs, Vector2d const& rhs)
{
  return lhs.x * rhs.x + lhs.y + rhs.y;
}
// scalar*vector
Vector2d operator*(double const& lhs, Vector2d const& rhs)
{
  Vector2d res{rhs};
  res.x *= lhs;
  res.y *= lhs;
  return res;
}
// vector*scalar
Vector2d operator*(Vector2d const& lhs, double const& rhs)
{
  return operator*(rhs, lhs);
}
// vector/scalar EXCEPTION IF rhs==0
Vector2d operator/(Vector2d const& lhs, double const& rhs)
{
  // da aggiungere l'EXCEPTION, ora non ho sbatti di cercare come si faceva
  return operator*(1 / rhs, lhs);
}

// sum between two vectors
Vector2d operator+(Vector2d const& lhs, Vector2d const& rhs)
{
  Vector2d sum{lhs};
  return sum += rhs;
}
// difference between two vectors
Vector2d operator-(Vector2d const& lhs, Vector2d const& rhs)
{}

// returns the norm squared of a vector
double norm2(Vector2d const& vec)
{
  return vec.x * vec.x + vec.y * vec.y;
}
// returns the norm of a vector
double norm(Vector2d const& vec)
{
  return std::sqrt(norm2(vec));
}

// rotate the vector by "angle" radians
Vector2d rotate(Vector2d const& vec, double angle)
{}

} // namespace kape
