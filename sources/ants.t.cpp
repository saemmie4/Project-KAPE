#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ants.hpp"
#include "doctest.h"
#include <cmath>
#include <numbers>

TEST_CASE("Testing the Ants class")
{
  std::default_random_engine eng;
  kape::Ant a1{{3., 2.5}, {3., 2.5}, false};
  std::array<kape::Circle, 3> cov1;
  a1.calculateCirclesOfVision(cov1);
  kape::Ant a2{{0.99, 4.46}, {0.99, 4.46}, false};
  std::array<kape::Circle, 3> cov2;
  a2.calculateCirclesOfVision(cov2);
  kape::Ant a3{{4.418, -0.495}, {4.418, -0.495}, false};
  std::array<kape::Circle, 3> cov3;
  a3.calculateCirclesOfVision(cov3);
  kape::Ant a4{{5., 1.}, {5., 1.}, false};
  std::array<kape::Circle, 3> cov4;
  a4.calculateCirclesOfVision(cov4);
  kape::Ant a5{{-1., 1.}, {-1., 1.}, false};
  std::array<kape::Circle, 3> cov5;
  a5.calculateCirclesOfVision(cov5);
  kape::Ant a6{{-3., -2.5}, {-3., -2.5}, false};
  std::array<kape::Circle, 3> cov6;
  a6.calculateCirclesOfVision(cov6);
  kape::Ant a7{{-0.99, -4.46}, {-0.99, -4.46}, false};
  std::array<kape::Circle, 3> cov7;
  a7.calculateCirclesOfVision(cov7);

  kape::Rectangle r1{{2., 6.}, 4., 5.};
  kape::Rectangle r2{{5., -1.5}, 5., 4.};
  kape::Rectangle r3{{-5., -1.}, 1., 3.};
  kape::Rectangle r4{{0., -3.}, 2., 3.};
  kape::Rectangle r5{{-2., -6.}, 4., 5.};

  kape::Obstacles obs;
  obs.addObstacle(r1);
  obs.addObstacle(r2);
  obs.addObstacle(r3);
  obs.addObstacle(r4);
  obs.addObstacle(r5);

SUBCASE("testing the calculateAngleToAvoidObstacles function") {
}
}