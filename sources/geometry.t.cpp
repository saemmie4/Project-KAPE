#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "geometry.hpp"
#include "doctest.h"
#include <numbers>

// TODO:
//  - tests for doShapesIntersect(Circle const& circle, Vector2d const& point);
//  - tests for doShapesIntersect(Rectangle const& rectangle, Vector2d const&
//   point);
//  - tests for doShapesIntersect(Circle const& circle, Rectangle const&
//   rectangle);
//
//  NEW:
//  - tests for doShapesInstersec(Circle const& circle1, Circle const& circle2)
//  -solve that operator/ makes tests fail because it (correctly) crashes the
//  program with the assert to check if the denominator is 0

TEST_CASE("Testing the Vector2d class")
{
  kape::Vector2d v1{1., 5.};
  kape::Vector2d v2{0., -5.};
  SUBCASE("testing norm function")
  {
    CHECK(norm(v1) == doctest::Approx(5.0990195));
    CHECK(norm(v2) == doctest::Approx(5.0));
  }

  SUBCASE("testing norm2 function")
  {
    CHECK(norm2(v1) == doctest::Approx(26.0));
    CHECK(norm2(v2) == doctest::Approx(25.0));
  }

  SUBCASE("testing rotate function")
  {}

  SUBCASE("testing operator +")
  {
    CHECK((v1 + v2).x == doctest::Approx(1.));
    CHECK((v1 + v2).y == doctest::Approx(0.));
    CHECK((v1 + v1 + v2 + v1).x == doctest::Approx(3.));
    CHECK((v1 + v1 + v2 + v1).y == doctest::Approx(10.));
  }

  SUBCASE("testing operator v-v")
  {
    CHECK((v1 - v2).x == doctest::Approx(1.));
    CHECK((v1 - v2).y == doctest::Approx(10.));
    CHECK((v1 - v1 - v2 - v1).x == doctest::Approx(-1.));
    CHECK((v1 - v1 - v2 - v1).y == doctest::Approx(0.));
  }

  SUBCASE("testing operator -v")
  {
    CHECK((-v1).x == doctest::Approx(-1.));
    CHECK((-v1).y == doctest::Approx(-5.));
    CHECK((-v2).x == doctest::Approx(0.));
    CHECK((-v2).y == doctest::Approx(5.));
  }

  SUBCASE("testing operator v*v")
  {
    kape::Vector2d v1perp{-v1.y, v1.x};
    CHECK(v1 * v1perp == doctest::Approx(0.));
    CHECK(v1 * v1 == doctest::Approx(26.0));
    CHECK(v2 * v2 == doctest::Approx(25.));
    CHECK(v1 * v2 == doctest::Approx(-25.));
  }

  SUBCASE("testing operator n*v")
  {
    CHECK((0. * v1).x == doctest::Approx(0.));
    CHECK((0. * v1).y == doctest::Approx(0.));
    CHECK((-3. * v2).x == doctest::Approx(0.));
    CHECK((-3. * v2).y == doctest::Approx(15.));
    CHECK((7. * v1).x == doctest::Approx(7.));
    CHECK((7. * v1).y == doctest::Approx(35.));
  }

  SUBCASE("testing operator v*n")
  {
    CHECK((v1 * 0.).x == doctest::Approx(0.));
    CHECK((v1 * 0.).y == doctest::Approx(0.));
    CHECK((v2 * (-3.)).x == doctest::Approx(0.));
    CHECK((v2 * (-3.)).y == doctest::Approx(15.));
    CHECK((v1 * 7.).x == doctest::Approx(7.));
    CHECK((v1 * 7.).y == doctest::Approx(35.));
  }

  SUBCASE("testing operator v/n")
  {
    CHECK((v1 / 2.).x == doctest::Approx(0.5));
    CHECK((v1 / 2.).y == doctest::Approx(2.5));
    CHECK((v2 / (-3.)).x == doctest::Approx(0.));
    CHECK((v2 / (-3.)).y == doctest::Approx(1.6666667));
    CHECK_THROWS(v1 / 0.);
    CHECK_THROWS(v2 / 0.);
  }

  SUBCASE("testing operator v+=v")
  {
    kape::Vector2d v{v1};
    CHECK((v += v2).y == doctest::Approx(0.));
    CHECK(v.x == doctest::Approx(1.));
    CHECK(v.y == doctest::Approx(0.));
  }
  SUBCASE("testing operator v-=v")
  {
    kape::Vector2d v{v1};
    CHECK((v -= v2).y == doctest::Approx(10.));
    CHECK(v.x == doctest::Approx(1.));
    CHECK(v.y == doctest::Approx(10.));
  }
  SUBCASE("testing operator v*=n")
  {
    kape::Vector2d v{v1};
    CHECK((v *= 10.).y == doctest::Approx(50.));
    CHECK(v.x == doctest::Approx(10.));
    CHECK(v.y == doctest::Approx(50.));
    CHECK((v *= 0.).y == doctest::Approx(0.));
    CHECK(v.x == doctest::Approx(0.));
    CHECK(v.y == doctest::Approx(0.));
    CHECK((v *= -100000.).y == doctest::Approx(0.));
    CHECK(v.x == doctest::Approx(0.));
    CHECK(v.y == doctest::Approx(0.));
  }
  SUBCASE("testing operator v/=n")
  {
    // kape::Vector2d v1{1., 5.};
    // kape::Vector2d v2{0., -5.};
    kape::Vector2d v{v1};
    CHECK((v /= 10.).y == doctest::Approx(0.5));
    CHECK(v.x == doctest::Approx(0.1));
    CHECK(v.y == doctest::Approx(0.5));
    CHECK_THROWS(v /= 0.);
    CHECK(v.x == doctest::Approx(0.1));
    CHECK(v.y == doctest::Approx(0.5));
    CHECK((v /= -0.1).y == doctest::Approx(-5.));
    CHECK(v.x == doctest::Approx(-1.));
    CHECK(v.y == doctest::Approx(-5.));
  }
  SUBCASE("Testing the rotate function")
  {
    // kape::Vector2d v1{1., 5.};
    // kape::Vector2d v2{0., -5.};
    CHECK(kape::rotate(v1, kape::PI).x == doctest::Approx(-1.));
    CHECK(kape::rotate(v1, kape::PI).y == doctest::Approx(-5.));
    CHECK(kape::rotate(v1, kape::PI / 2.).x == doctest::Approx(-5.));
    CHECK(kape::rotate(v1, kape::PI / 2.).y == doctest::Approx(1.));
    CHECK(kape::rotate(v1, kape::PI / 4.).x
          == doctest::Approx(-2. * std::sqrt(2.)));
    CHECK(kape::rotate(v1, kape::PI / 4.).y
          == doctest::Approx(3. * std::sqrt(2.)));
    CHECK(kape::rotate(v1, 0.).x == doctest::Approx(1.));
    CHECK(kape::rotate(v1, 0.).y == doctest::Approx(5.));
    CHECK(kape::rotate(v1, kape::PI / 3.).x
          == doctest::Approx(rotate(v1, 7 * kape::PI / 3.).x));
    CHECK(rotate(v1, -(kape::PI) / 3.).x == doctest::Approx(4.830127));
    CHECK(rotate(v1, -(kape::PI) / 3.).y == doctest::Approx(1.633975));
  }
}

TEST_CASE("Testing Circle class")
{
  kape::Circle c1{{1., 1.}, 1.};
  kape::Vector2d v1{-1., 0.};
  kape::Vector2d v2{1., 1.};
  kape::Vector2d v3{0., 1.};
  SUBCASE("testing isInside function")
  {
    CHECK(c1.isInside(v1) == false);
    CHECK(c1.isInside(v2) == true);
    CHECK(c1.isInside(v3) == true);
  }
}

TEST_CASE("Testing doShapesIntersect")
{
  kape::Vector2d v1{-3., 1.5};
  kape::Vector2d v2{0.5, 3.};
  kape::Vector2d v3{-3., 5.};
  kape::Vector2d v4{1.5, 1};
  kape::Circle c1{{1., 1.}, 1.};
  kape::Rectangle r1{{-1., 2.}, 2., 3.};
  kape::Circle c2{{-3., 4.}, 3.};
  kape::Rectangle r2{{-1., 4.}, 2., 4.};
  kape::Circle c3{{-3., 4.}, 2.};
  kape::Rectangle r3{{3., 6.}, 3., 4.};
  kape::Circle c4{{1., 0.}, std::sqrt(2.) * 2.};
  kape::Circle c5{{0., 1}, 0.5};
  CHECK(doShapesIntersect(c2, v1) == true);
  CHECK(doShapesIntersect(c3, v1) == false);
  CHECK(doShapesIntersect(r2, v2) == true);
  CHECK(doShapesIntersect(c1, v4) == true);
  CHECK(doShapesIntersect(c3, r2) == true);
  CHECK(doShapesIntersect(c1, r3) == false);
  CHECK(doShapesIntersect(c4, r3) == true);
  CHECK(doShapesIntersect(c3, v3) == true);
  CHECK(doShapesIntersect(c5, r1) == true);
  CHECK(doShapesIntersect(c1, r1) == true);
}