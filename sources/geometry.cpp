#include "geometry.hpp"
#include <cassert>   //per assert
#include <cmath>     //for std::sqrt
#include <stdexcept> //for std::domain_error
// TODO:
// - rectangle implementation
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
  return lhs.x * rhs.x + lhs.y * rhs.y;
}
// scalar*vector
Vector2d operator*(double const& lhs, Vector2d const& rhs)
{
  return Vector2d{lhs * rhs.x, lhs * rhs.y};
}

// vector*scalar
Vector2d operator*(Vector2d const& lhs, double const& rhs)
{
  return rhs * lhs;
}

// vector/scalar
// may throw a std::domain_error if rhs==0 (division by 0)
Vector2d operator/(Vector2d const& lhs, double const& rhs)
{
  if (rhs == 0.) {
    throw std::domain_error{"the denominator can't be 0"};
  }

  return (1. / rhs) * lhs;
}

// sum between two vectors
Vector2d operator+(Vector2d const& lhs, Vector2d const& rhs)
{
  Vector2d sum{lhs};
  return sum += rhs;
}

// opposite of a vector
Vector2d operator-(Vector2d const& rhs)
{
  return (-1.) * rhs;
}

// difference between two vectors
Vector2d operator-(Vector2d const& lhs, Vector2d const& rhs)
{
  return lhs + (-rhs);
}

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
{
  return Vector2d{vec.x * std::cos(angle) - vec.y * std::sin(angle),
                  vec.x * std::sin(angle) + vec.y * std::cos(angle)};
}

// Circle implementation----------------------------------

Circle::Circle(Vector2d const& center, double radius)
    : center_{center}
    , radius_{radius}
{
  if (radius <= 0) {
    throw std::invalid_argument{"The radius can't be negative or null"};
  }
}

Vector2d Circle::getCircleCenter() const
{
  return center_;
}

double Circle::getCircleRadius() const
{
  return radius_;
}

bool Circle::isInside(Vector2d const& position)
{
  return norm2(center_ - position) <= radius_ * radius_;
}

// Rectangle Implementation-----------------------------------

} // namespace kape
