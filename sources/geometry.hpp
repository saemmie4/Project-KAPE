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
  Vector2d& operator-=(Vector2d const& rhs);
  Vector2d& operator*=(double rhs);
  // may throw a std::domain_error if rhs==0 (division by 0)
  Vector2d& operator/=(double rhs);
};

// dot product
double operator*(Vector2d const& lhs, Vector2d const& rhs);
// scalar*vector
Vector2d operator*(double lhs, Vector2d const& rhs);
// vector*scalar
Vector2d operator*(Vector2d const& lhs, double rhs);
// vector/scalar.
// may throw a std::domain_error if rhs==0 (division by 0)
Vector2d operator/(Vector2d const& lhs, double rhs);
// sum between two vectors
Vector2d operator+(Vector2d const& lhs, Vector2d const& rhs);
// opposite of a vector
Vector2d operator-(Vector2d const& rhs);
// difference between two vectors
Vector2d operator-(Vector2d const& lhs, Vector2d const& rhs);

// returns the norm squared of a vector
double norm2(Vector2d const& vec);

// returns the norm of a vector
double norm(Vector2d const& vec);

// rotate the vector by "angle" radians
Vector2d rotate(Vector2d const& vec, double angle);

class Circle
{
  Vector2d center_;
  double radius_;

 public:
  Circle(Vector2d const& center, double radius);
  Vector2d getCircleCenter() const;
  double getCircleRadius() const;
  bool isInside(Vector2d const& position) const;
};

class Rectangle
{
  Vector2d top_left_corner_;
  double width_;
  double height_;

 public:
  Rectangle(Vector2d const& top_left_corner, double width, double height);
  Vector2d getRectangleTopLeftCorner() const;
  double getRectangleWidth() const;
  double getRectangleHeight() const;
};

//true: they intersect
//false: they don't
bool DoShapesIntersect(Circle const& circle, Vector2d const& point);
bool DoShapesIntersect(Rectangle const& rectangle, Vector2d const& point);
bool DoShapesIntersect(Circle const& circle, Rectangle const& rectangle);

} // namespace kape

#endif