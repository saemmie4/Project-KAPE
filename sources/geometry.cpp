#include "geometry.hpp"
#include <cassert>   //per assert
#include <cmath>     //for std::sqrt
#include <stdexcept> //for std::domain_error

// TODO:
// - rectangle implementation
namespace kape {

Vector2d& Vector2d::operator+=(Vector2d const& rhs)
{
  *this = *this + rhs;
  return *this;
}

Vector2d& Vector2d::operator-=(Vector2d const& rhs)
{
  *this = *this - rhs;
  return *this;
}
Vector2d& Vector2d::operator*=(double rhs)
{
  *this = *this * rhs;
  return *this;
}
// may throw a std::domain_error if rhs==0 (division by 0)
Vector2d& Vector2d::operator/=(double rhs)
{
  *this = *this / rhs;
  return *this;
}

// dot product
double operator*(Vector2d const& lhs, Vector2d const& rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}
// scalar*vector
Vector2d operator*(double lhs, Vector2d const& rhs)
{
  return Vector2d{lhs * rhs.x, lhs * rhs.y};
}

// vector*scalar
Vector2d operator*(Vector2d const& lhs, double rhs)
{
  return rhs * lhs;
}

// vector/scalar
// may throw a std::domain_error if rhs==0 (division by 0)
Vector2d operator/(Vector2d const& lhs, double rhs)
{
  if (rhs == 0.) {
    throw std::domain_error{"the denominator can't be 0"};
  }

  return (1. / rhs) * lhs;
}

// sum between two vectors
Vector2d operator+(Vector2d const& lhs, Vector2d const& rhs)
{
  return Vector2d{lhs.x + rhs.x, lhs.y + rhs.y};
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

void Circle::setCircleCenter(Vector2d const& center)
{
  center_ = center;
}
bool Circle::isInside(Vector2d const& position) const
{
  return doShapesIntersect(*this, position);
}

// Rectangle Implementation-----------------------------------

Rectangle::Rectangle(Vector2d const& top_left_corner, double width,
                     double height)
    : top_left_corner_{top_left_corner}
    , width_{width}
    , height_{height}
{
  if (width <= 0)
    throw std::invalid_argument{"The width can't be negative or null"};
  if (height <= 0)
    throw std::invalid_argument{"The height can't be negative or null"};
};

Vector2d Rectangle::getRectangleTopLeftCorner() const
{
  return top_left_corner_;
}
double Rectangle::getRectangleWidth() const
{
  return width_;
}
double Rectangle::getRectangleHeight() const
{
  return height_;
}

void Rectangle::setRectangleTopLeftCorner(Vector2d const& top_left_corner)
{
  top_left_corner_ = top_left_corner;
}

bool doShapesIntersect(Circle const& circle, Vector2d const& point)
{
  return norm2(circle.getCircleCenter() - point)
      <= circle.getCircleRadius() * circle.getCircleRadius();
}

// true: point x is between left and right edge
// false: else
bool isPointBetweenLeftAndRightEdge(Rectangle const& rectangle,
                                    Vector2d const& point)
{
  Vector2d const tlc = rectangle.getRectangleTopLeftCorner();
  double const w     = rectangle.getRectangleWidth();

  return (std::abs(point.x - tlc.x - w / 2.) <= w / 2.);
}

// true: point y is between top and bottom edge
// false: else
bool isPointBetweenTopAndBottomEdge(Rectangle const& rectangle,
                                    Vector2d const& point)
{
  Vector2d const tlc = rectangle.getRectangleTopLeftCorner();
  double const h     = rectangle.getRectangleHeight();

  return (std::abs(-point.y + tlc.y - h / 2.) <= h / 2.);
}

bool doShapesIntersect(Rectangle const& rectangle, Vector2d const& point)
{
  return isPointBetweenLeftAndRightEdge(rectangle, point)
      && isPointBetweenTopAndBottomEdge(rectangle, point);
}

bool doShapesIntersect(Circle const& circle, Rectangle const& rectangle)
{
  Vector2d const c   = circle.getCircleCenter();
  double const r     = circle.getCircleRadius();
  double const w     = rectangle.getRectangleWidth();
  double const h     = rectangle.getRectangleHeight();
  Vector2d const tlc = rectangle.getRectangleTopLeftCorner();
  Vector2d const trc = tlc + Vector2d{w, 0.};
  Vector2d const brc = trc + Vector2d{0., -h};
  Vector2d const blc = brc + Vector2d{-w, 0.};

  bool intersect{false};

  // left side
  if (isPointBetweenTopAndBottomEdge(rectangle, circle.getCircleCenter())) {
    intersect = intersect || (std::abs(c.x - tlc.x) <= r);
  } else {
    intersect =
        intersect
        || (doShapesIntersect(circle, tlc) || doShapesIntersect(circle, blc));
  }

  // skip other checks if they already intersect
  if (intersect) {
    return true;
  }

  // right side
  if (isPointBetweenTopAndBottomEdge(rectangle, circle.getCircleCenter())) {
    intersect = intersect || (std::abs(c.x - trc.x) <= r);
  } else {
    intersect =
        intersect
        || (doShapesIntersect(circle, trc) || doShapesIntersect(circle, brc));
  }

  // skip other checks if they already intersect
  if (intersect) {
    return true;
  }

  // top side
  if (isPointBetweenLeftAndRightEdge(rectangle, circle.getCircleCenter())) {
    intersect = intersect || (std::abs(c.y - tlc.y) <= r);
  } else {
    intersect =
        intersect
        || (doShapesIntersect(circle, tlc) || doShapesIntersect(circle, trc));
  }

  // skip other checks if they already intersect
  if (intersect) {
    return true;
  }

  // bottom side
  if (isPointBetweenLeftAndRightEdge(rectangle, circle.getCircleCenter())) {
    intersect = intersect || (std::abs(c.y - blc.y) <= r);
  } else {
    intersect =
        intersect
        || (doShapesIntersect(circle, blc) || doShapesIntersect(circle, brc));
  }

  // skip other checks if they already intersect
  if (intersect) {
    return true;
  }

  // edge case: circle all inside rectangle
  // check if circle's center is inside the rectangle;
  // if it isn't (and, because we are here, the function didn't return early)
  // they must not intersect
  return doShapesIntersect(rectangle, c);
}

} // namespace kape
