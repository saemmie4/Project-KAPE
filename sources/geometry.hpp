#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

// TODO:
// operator-=
// operator*=
// operator/=

namespace kape {
struct Vector2d
{
  double x;
  double y;

  Vector2d& operator+=(Vector2d const& rhs);
};

// dot product
double operator*(Vector2d const& lhs, Vector2d const& rhs);
// scalar*vector
Vector2d operator*(double const& lhs, Vector2d const& rhs);
// vector*scalar
Vector2d operator*(Vector2d const& lhs, double const& rhs);
// vector/scalar EXCEPTION IF rhs==0
Vector2d operator/(Vector2d const& lhs, double const& rhs);
// sum between two vectors
Vector2d operator+(Vector2d const& lhs, Vector2d const& rhs);
// difference between two vectors
Vector2d operator-(Vector2d const& lhs, Vector2d const& rhs);
// opposite of a vector
Vector2d operator-(Vector2d const& rhs);

// returns the norm squared of a vector
double norm2(Vector2d const& vec);

// returns the norm of a vector
double norm(Vector2d const& vec);

// rotate the vector by "angle" radians
Vector2d rotate(Vector2d const& vec, double angle);

} // namespace kape

#endif