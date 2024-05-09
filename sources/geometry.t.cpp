#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "geometry.hpp"
#include "doctest.h"

// TODO:
//  -tests for rotate
//  -tests for -=
//  -tests for *=
//  -tests for /=
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
}