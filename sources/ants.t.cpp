#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ants.hpp"
#include "doctest.h"
#include <cmath>
#include <numbers>

TEST_CASE("Testing the Ants class")
{
  std::default_random_engine eng;
  kape::Ant a1{kape::Vector2d{3., 2.5}, kape::Vector2d{3., 2.5}, false};
  std::array<kape::Circle, 3> cov1;
  a1.calculateCirclesOfVision(cov1);
  kape::Ant a2{kape::Vector2d{0.99, 4.46}, kape::Vector2d{0.99, 4.46}, false};
  std::array<kape::Circle, 3> cov2;
  a2.calculateCirclesOfVision(cov2);
  kape::Ant a3{kape::Vector2d{4.418, -0.495}, kape::Vector2d{4.418, -0.495},
               false};
  std::array<kape::Circle, 3> cov3;
  a3.calculateCirclesOfVision(cov3);
  kape::Ant a4{kape::Vector2d{5., 1.}, kape::Vector2d{5., 1.}, false};
  std::array<kape::Circle, 3> cov4;
  a4.calculateCirclesOfVision(cov4);
  kape::Ant a5{kape::Vector2d{-1., 1.}, kape::Vector2d{-1., 1.}, false};
  std::array<kape::Circle, 3> cov5;
  a5.calculateCirclesOfVision(cov5);
  kape::Ant a6{kape::Vector2d{-3., -2.5}, kape::Vector2d{-3., -2.5}, false};
  std::array<kape::Circle, 3> cov6;
  a6.calculateCirclesOfVision(cov6);
  kape::Ant a7{kape::Vector2d{-0.99, -4.46}, kape::Vector2d{-0.99, -4.46},
               false};
  std::array<kape::Circle, 3> cov7;
  a7.calculateCirclesOfVision(cov7);

  kape::Rectangle r1{kape::Vector2d{2., 6.}, 4., 5.};
  kape::Rectangle r2{kape::Vector2d{5., -1.5}, 5., 4.};
  kape::Rectangle r3{kape::Vector2d{-5., -1.}, 1., 3.};
  kape::Rectangle r4{kape::Vector2d{0., -3.}, 2., 3.};
  kape::Rectangle r5{kape::Vector2d{-2., -6.}, 4., 5.};

  kape::Obstacles obs;
  obs.addObstacle(r1);
  obs.addObstacle(r2);
  obs.addObstacle(r3);
  obs.addObstacle(r4);
  obs.addObstacle(r5);

  SUBCASE("testing the calculateAngleToAvoidObstacles function")
  {
    CHECK(a2.calculateAngleToAvoidObstacles(cov2, obs, eng)
          == doctest::Approx(kape::PI / 6.));
  }

  SUBCASE("testing the getFacingAngle function")
  {
    CHECK(a1.getFacingAngle() == doctest::Approx(0.695));
    CHECK(a3.getFacingAngle() == doctest::Approx(-0.112));
    CHECK(a5.getFacingAngle() == doctest::Approx(0.695 + kape::PI));
  }
}
