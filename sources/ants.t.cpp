#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ants.hpp"
#include "doctest.h"
#include <cmath>
#include <numbers>

TEST_CASE("Testing the Ants class")
{
  std::default_random_engine eng;
  kape::Ant a1{kape::Vector2d{0.006, 0.015}, kape::Vector2d{0.006, 0.015},
               false};
  std::array<kape::Circle, 3> cov1;
  a1.calculateCirclesOfVision(cov1);
  kape::Ant a2{kape::Vector2d{0.0075, 0.0075}, kape::Vector2d{0.0075, 0.0075},
               false};
  std::array<kape::Circle, 3> cov2;
  a2.calculateCirclesOfVision(cov2);
  kape::Ant a3{kape::Vector2d{0.015, 0.006}, kape::Vector2d{0.015, 0.006},
               false};
  std::array<kape::Circle, 3> cov3;
  a3.calculateCirclesOfVision(cov3);
  kape::Ant a4{kape::Vector2d{0.01, -0.005}, kape::Vector2d{0.01, -0.05},
               false};
  std::array<kape::Circle, 3> cov4;
  a4.calculateCirclesOfVision(cov4);
  kape::Ant a5{kape::Vector2d{0.005, -0.01}, kape::Vector2d{0.005, -0.01},
               false};
  std::array<kape::Circle, 3> cov5;
  a5.calculateCirclesOfVision(cov5);
  kape::Ant a6{kape::Vector2d{0., -0.015}, kape::Vector2d{0., -0.015}, false};
  std::array<kape::Circle, 3> cov6;
  a6.calculateCirclesOfVision(cov6);
  kape::Ant a7{kape::Vector2d{-0.01, 0.}, kape::Vector2d{-0.01, 0.}, false};
  std::array<kape::Circle, 3> cov7;
  a7.calculateCirclesOfVision(cov7);
  kape::Ant a8{kape::Vector2d{-0.006, 0.015}, kape::Vector2d{-0.006, 0.015},
               false};
  std::array<kape::Circle, 3> cov8;
  a8.calculateCirclesOfVision(cov8);
  kape::Ant a9{kape::Vector2d{-0.006, 0.015}, kape::Vector2d{-0.006, 0.015},
               true};
  std::array<kape::Circle, 3> cov9;
  a9.calculateCirclesOfVision(cov9);

  kape::Rectangle r1{kape::Vector2d{0.01, 0.0175}, 0.0075, 0.0075};
  kape::Rectangle r2{kape::Vector2d{0.01, -0.01}, 0.01, 0.01};
  kape::Rectangle r3{kape::Vector2d{0.006, -0.022}, 0.014, 0.014};
  kape::Rectangle r4{kape::Vector2d{-0.02, -0.022}, 0.014, 0.014};
  kape::Rectangle r5{kape::Vector2d{-0.02, 0.01}, 0.01, 0.02};

  kape::Obstacles obs;
  obs.addObstacle(r1);
  obs.addObstacle(r2);
  obs.addObstacle(r3);
  obs.addObstacle(r4);
  obs.addObstacle(r5);

  kape::PheromoneParticle part1{kape::Vector2d{-0.004, 0.022}, 50.};
  kape::PheromoneParticle part2{kape::Vector2d{-0.01, 0.024}, 25.};
  kape::PheromoneParticle part3{kape::Vector2d{-0.012, 0.016}, 100.};

  kape::Pheromones to_food_pheromones{
      kape::Pheromones::Type::TO_FOOD,
      kape::Ant::CIRCLE_OF_VISION_RADIUS * 2.,
  };
  kape::Pheromones to_anthill_pheromones{
      kape::Pheromones::Type::TO_ANTHILL,
      kape::Ant::CIRCLE_OF_VISION_RADIUS * 2.,
  };
  to_food_pheromones.addPheromoneParticle(part1);
  to_food_pheromones.addPheromoneParticle(part2);
  to_anthill_pheromones.addPheromoneParticle(part3);
  kape::Circle c1{kape::Vector2d{0., 0.}, 1.};
  kape::Circle c2{kape::Vector2d{1., 1.}, 1.};
  std::size_t n0{0};
  std::size_t n1{1};
  std::size_t n2{10};
  std::size_t n3{100};

  kape::Ants ants{};

  SUBCASE("testing the calculateCirclesOfVision function")
  {
    CHECK(cov1[0].getCircleCenter().x == doctest::Approx(0.00136207));
    CHECK(cov1[0].getCircleCenter().y == doctest::Approx(0.020894));
    CHECK(cov1[1].getCircleCenter().x == doctest::Approx(0.00878543));
    CHECK(cov1[2].getCircleCenter().y == doctest::Approx(0.0160695));
    CHECK(cov6[1].getCircleCenter().y == doctest::Approx(-0.0225));
    CHECK(cov1[1].getCircleRadius() == doctest::Approx(0.003846));
  }

  SUBCASE("testing the calculateAngleToAvoidObstacles function")
  {
    CHECK(a1.calculateAngleToAvoidObstacles(cov1, obs, eng)
          == doctest::Approx(kape::PI / 6.));
    CHECK(a3.calculateAngleToAvoidObstacles(cov3, obs, eng)
          == doctest::Approx(-kape::PI / 6.));
    CHECK(a4.calculateAngleToAvoidObstacles(cov4, obs, eng)
          == doctest::Approx(5 * kape::PI / 3.));
    CHECK(a5.calculateAngleToAvoidObstacles(cov5, obs, eng)
          == doctest::Approx(5 * kape::PI / 3.));
    CHECK(a6.calculateAngleToAvoidObstacles(cov6, obs, eng)
          == doctest::Approx(0.));
    CHECK(a7.calculateAngleToAvoidObstacles(cov7, obs, eng)
          == doctest::Approx(kape::PI));
  }

  SUBCASE("testing the applyPheromonesInfluence function")
  {
    a8.applyPheromonesInfluence(cov8, to_food_pheromones);
    CHECK(a8.getDesiredDirection().x == doctest::Approx(0.274721));
    CHECK(a8.getDesiredDirection().y == doctest::Approx(0.961524));
    a9.applyPheromonesInfluence(cov9, to_anthill_pheromones);
    CHECK(a9.getDesiredDirection().x == doctest::Approx(-0.986394));
    CHECK(a9.getDesiredDirection().y == doctest::Approx(0.164399));
  }

  SUBCASE("testing the addAntsAroundCircle function")
  {
    ants.addAntsAroundCircle(c1, n0);
    CHECK(ants.getNumberOfAnts() == 0);
    ants.addAntsAroundCircle(c1, n3);
    CHECK(ants.getNumberOfAnts() == 100);
    ants.addAntsAroundCircle(c2, n1);
    ants.addAntsAroundCircle(c2, n2);
    CHECK(ants.getNumberOfAnts() == 111);
  }
}