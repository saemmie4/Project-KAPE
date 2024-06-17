#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "environment.hpp"
#include "doctest.h"

TEST_CASE("Testing Obstacles class")
{
  kape::Obstacles obstacles;
  obstacles.addObstacle(kape::Vector2d{1., 1.5}, 2., 0.5);
  obstacles.addObstacle(kape::Vector2d{-3, 0}, 1.5, 1);
  obstacles.addObstacle(kape::Rectangle{kape::Vector2d{-1., 3.}, 0.2, 3.});
  obstacles.addObstacle(kape::Rectangle{kape::Vector2d{4., 1.}, 1., 4.});
  SUBCASE("Testing addObstacle and getNumberOfObstacles function")
  {
    CHECK(obstacles.getNumberOfObstacles() == 4);
    obstacles.addObstacle(kape::Vector2d{2., -1.}, 0.5, 1.);
    CHECK(obstacles.getNumberOfObstacles() == 5);
  }
  SUBCASE("Testing anyObstacleInCircle function")
  {
    kape::Circle c1{kape::Vector2d{1., 0.}, 0.5};
    kape::Circle c2{kape::Vector2d{3.5, 1.5}, 1.};
    kape::Circle c3{kape::Vector2d{2.5, -1.}, 1.5};
    kape::Circle c4{kape::Vector2d{3.5, -3.}, 0.5};
    CHECK(obstacles.anyObstaclesInCircle(c1) == false);
    CHECK(obstacles.anyObstaclesInCircle(c2) == true);
    CHECK(obstacles.anyObstaclesInCircle(c3) == true);
    CHECK(obstacles.anyObstaclesInCircle(c4) == true);
  }
  SUBCASE("Testing const iterators begin && end")
  {
    int number_of_obstacles{0};
    for (auto it = obstacles.begin(), end = obstacles.end(); it != end; ++it) {
      ++number_of_obstacles;
    }
    CHECK(number_of_obstacles == 4);
  }
}

TEST_CASE("Testing FoodParticle class")
{
  kape::FoodParticle f1{kape::Vector2d{5.3, 2.}};
  kape::FoodParticle f2{kape::Vector2d{-3.1, 17.3}};
  kape::FoodParticle f3{kape::Vector2d{-13.5, -24.6}};
  kape::FoodParticle f4{kape::Vector2d{10.7, -3.5}};
  kape::FoodParticle f5{kape::Vector2d{0., 0.}};
  kape::FoodParticle f6{kape::Vector2d{3690., 0.}};
  kape::FoodParticle f7{kape::Vector2d{0., 5062.}};
  SUBCASE("Testing getPosition function")
  {
    CHECK(f1.getPosition().x == doctest::Approx(5.3));
    CHECK(f1.getPosition().y == doctest::Approx(2.));
    CHECK(f2.getPosition().x == doctest::Approx(-3.1));
    CHECK(f2.getPosition().y == doctest::Approx(17.3));
    CHECK(f3.getPosition().x == doctest::Approx(-13.5));
    CHECK(f3.getPosition().y == doctest::Approx(-24.6));
    CHECK(f4.getPosition().x == doctest::Approx(10.7));
    CHECK(f4.getPosition().y == doctest::Approx(-3.5));
    CHECK(f5.getPosition().x == doctest::Approx(0.));
    CHECK(f5.getPosition().y == doctest::Approx(0.));
    CHECK(f6.getPosition().x == doctest::Approx(3690.));
    CHECK(f6.getPosition().y == doctest::Approx(0.));
    CHECK(f7.getPosition().x == doctest::Approx(0.));
    CHECK(f7.getPosition().y == doctest::Approx(5062.));
  }
}

TEST_CASE("Testing PheromoneParticle class")
{
  kape::PheromoneParticle p1{kape::Vector2d{5.3, 2.}, 14};
  kape::PheromoneParticle p2{kape::Vector2d{-3.1, 17.3}, 7};
  kape::PheromoneParticle p3{kape::Vector2d{-13.5, -24.6}, 68};
  kape::PheromoneParticle p4{kape::Vector2d{10.7, -3.5}, 54};
  kape::PheromoneParticle p5{kape::Vector2d{0., 0.}, 2};
  kape::PheromoneParticle p6{kape::Vector2d{3690., 0.}, 1};
  kape::PheromoneParticle p7{kape::Vector2d{0., 5062.}, 100};
  SUBCASE("Testing the exception in PheromoneParticle constructor")
  {
    CHECK_THROWS(kape::PheromoneParticle{kape::Vector2d{6.7, 5.}, 0});
    CHECK_THROWS(kape::PheromoneParticle{kape::Vector2d{6.7, 5.}, -3592});
  }
  SUBCASE("Testing getPosition function")
  {
    CHECK(p1.getPosition().x == doctest::Approx(5.3));
    CHECK(p1.getPosition().y == doctest::Approx(2.));
    CHECK(p2.getPosition().x == doctest::Approx(-3.1));
    CHECK(p2.getPosition().y == doctest::Approx(17.3));
    CHECK(p3.getPosition().x == doctest::Approx(-13.5));
    CHECK(p3.getPosition().y == doctest::Approx(-24.6));
    CHECK(p4.getPosition().x == doctest::Approx(10.7));
    CHECK(p4.getPosition().y == doctest::Approx(-3.5));
    CHECK(p5.getPosition().x == doctest::Approx(0.));
    CHECK(p5.getPosition().y == doctest::Approx(0.));
    CHECK(p6.getPosition().x == doctest::Approx(3690.));
    CHECK(p6.getPosition().y == doctest::Approx(0.));
    CHECK(p7.getPosition().x == doctest::Approx(0.));
    CHECK(p7.getPosition().y == doctest::Approx(5062.));
  }
  SUBCASE("Testing getIntensity function")
  {
    CHECK(p1.getIntensity() == 14);
    CHECK(p2.getIntensity() == 7);
    CHECK(p3.getIntensity() == 68);
    CHECK(p4.getIntensity() == 54);
    CHECK(p5.getIntensity() == 2);
    CHECK(p6.getIntensity() == 1);
    CHECK(p7.getIntensity() == 100);
  }
  SUBCASE("Testing decreaseIntensity function")
  {
    p1.decreaseIntensity();
    p2.decreaseIntensity(0.);
    p3.decreaseIntensity(0.99);
    p4.decreaseIntensity(0.001);
    CHECK(p1.getIntensity() == 14 * (1 - 0.005));
    CHECK(p2.getIntensity() == 7 * (1 - 0));
    CHECK(p3.getIntensity() == 68 * (1 - 0.99));
    CHECK(p4.getIntensity() == 54 * (1 - 0.001));
    CHECK_THROWS(p5.decreaseIntensity(56));
    CHECK_THROWS(p6.decreaseIntensity(1));
    CHECK_THROWS(p7.decreaseIntensity(-10));
  }
  SUBCASE("Testing hasEvaporated function")
  {
    p1.decreaseIntensity();
    p2.decreaseIntensity(0.);
    p3.decreaseIntensity(0.99);
    p4.decreaseIntensity(0.001);
    p5.decreaseIntensity(0.99);
    p5.decreaseIntensity(0.99);
    CHECK(p1.hasEvaporated() == false);
    CHECK(p2.hasEvaporated() == false);
    CHECK(p3.hasEvaporated() == false);
    CHECK(p4.hasEvaporated() == false);
    CHECK(p5.hasEvaporated() == true);
  }
}

TEST_CASE("Testing Food class")
{
  kape::Food food(12);
  kape::Obstacles obstacles;
  obstacles.addObstacle(kape::Vector2d{1., 1.5}, 2., 0.5);
  obstacles.addObstacle(kape::Vector2d{-3, 0}, 1.5, 1);
  obstacles.addObstacle(kape::Rectangle{kape::Vector2d{-1., 3.}, 0.2, 3.});
  obstacles.addObstacle(kape::Rectangle{kape::Vector2d{4., 1.}, 1., 4.});
  food.generateFoodInCircle(kape::Circle{kape::Vector2d{1., -1.}, 1.}, 50,
                            obstacles);
  food.generateFoodInCircle(kape::Circle{kape::Vector2d{3, 2.}, 1.4}, 100,
                            obstacles);
  food.generateFoodInCircle(kape::Circle{kape::Vector2d{3.5, -3.5}, 0.708}, 25,
                            obstacles);
  SUBCASE("Testing getNumberOfFoodParticles and generateFoodInCircle functions")
  {
    CHECK(food.getNumberOfFoodParticles() == 50);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{10., 15.}, 0.5}, 73,
                              obstacles);
    CHECK(food.getNumberOfFoodParticles() == 123);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{3.5, -3.}, 0.5}, 197,
                              obstacles);
    CHECK(food.getNumberOfFoodParticles() == 123);
  }
  SUBCASE("Testing removeOneFoodParticleInCircle")
  {
    food.removeOneFoodParticleInCircle(
        kape::Circle{kape::Vector2d{2., -1.}, 1.});
    CHECK(food.getNumberOfFoodParticles() == 49);
    food.removeOneFoodParticleInCircle(
        kape::Circle{kape::Vector2d{1., -2.5}, 1.});
    CHECK(food.getNumberOfFoodParticles() == 48);
    food.removeOneFoodParticleInCircle(
        kape::Circle{kape::Vector2d{1., -2.5}, 0.8});
    CHECK(food.getNumberOfFoodParticles() == 48);
  }
  SUBCASE("Testing isThereFoodLeft function")
  {
    CHECK(food.isThereFoodLeft() == true);
    food.removeOneFoodParticleInCircle(
        kape::Circle{kape::Vector2d{1., -1.}, 1.});
    CHECK(food.isThereFoodLeft() == true);
    for (int i{0}; i != 50; ++i) {
      food.removeOneFoodParticleInCircle(
          kape::Circle{kape::Vector2d{1., -1.}, 1.});
    }
    CHECK(food.isThereFoodLeft() == false);
  }
  SUBCASE("Testing const iterators begin && end")
  {
    int number_of_food_particles{0};
    for (auto food_it = food.begin(), food_end = food.end();
         food_it != food_end; ++food_it) {
      ++number_of_food_particles;
    }
    CHECK(number_of_food_particles == 50);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{100., 43.}, 0.5}, 73,
                              obstacles);
    for (auto food_it = food.begin(), food_end = food.end();
         food_it != food_end; ++food_it) {
      ++number_of_food_particles;
    }
    CHECK(number_of_food_particles == 173);
  }
}

TEST_CASE("Testing Iterator class")
{
  kape::Food food;
  kape::Obstacles obstacles;
  obstacles.addObstacle(kape::Vector2d{1., 1.5}, 2., 0.5);
  obstacles.addObstacle(kape::Vector2d{-3, 0}, 1.5, 1);
  obstacles.addObstacle(kape::Rectangle{kape::Vector2d{-1., 3.}, 0.2, 3.});
  obstacles.addObstacle(kape::Rectangle{kape::Vector2d{4., 1.}, 1., 4.});

  SUBCASE("Testing operator++")
  {
    int food_particles_counter{0};
    for (auto food_it = food.begin(), food_end = food.end();
         food_it != food_end; ++food_it) {
      ++food_particles_counter;
    }
    CHECK(food_particles_counter == 0);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{1., -1.}, 1.}, 50,
                              obstacles);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{3, 2.}, 1.4}, 100,
                              obstacles);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{3.5, -3.5}, 0.708},
                              25, obstacles);
    for (auto food_it = food.begin(), food_end = food.end();
         food_it != food_end; ++food_it) {
      ++food_particles_counter;
    }
    CHECK(food_particles_counter == 50);
  }
  SUBCASE("Testing operator*")
  {
    kape::Circle expected_circle{kape::Vector2d{1., -1.}, 1.};
    bool is_food_in_circle{false};
    for (auto food_it = food.begin(), food_end = food.end();
         food_it != food_end; ++food_it) {
      if (expected_circle.isInside((*food_it).getPosition())) {
        is_food_in_circle = true;
        break;
      }
    }
    CHECK(is_food_in_circle == false);
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{1., -1.}, 1.}, 50,
                              obstacles);
    for (auto food_it = food.begin(), food_end = food.end();
         food_it != food_end; ++food_it) {
      if (expected_circle.isInside((*food_it).getPosition())) {
        is_food_in_circle = true;
        break;
      }
    }
    CHECK(is_food_in_circle == true);
  }
  SUBCASE("Testing operator==")
  {
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{1., -1.}, 1.}, 50,
                              obstacles);
    kape::Food::Iterator food_it1{food.begin()};
    kape::Food::Iterator food_it2{++food_it1};
    kape::Food::Iterator food_it3{++food_it2};
    CHECK((food_it1 == food_it2) == false);
    CHECK((++food_it2 == (++food_it2)) == true);
    CHECK((food_it3 == food_it3) == true);
    CHECK((++food_it2 == food_it1) == false);
  }
  SUBCASE("Testing operator!=")
  {
    food.generateFoodInCircle(kape::Circle{kape::Vector2d{1., -1.}, 1.}, 50,
                              obstacles);
    kape::Food::Iterator food_it1{food.begin()};
    kape::Food::Iterator food_it2{++food_it1};
    kape::Food::Iterator food_it3{++food_it2};
    CHECK((food_it1 != food_it2) == true);
    CHECK((food_it2 != food_it2) == false);
    CHECK((food.begin() != food_it2) == true);
    CHECK((food.end() != food_it3) == true);
    CHECK((++food_it3 != ++food_it3) == false);
  }
}

TEST_CASE("Testing Pheromones class")
{
  // the ant's ant_circle_of_vision_diameter is set to 1. in these tests because
  // it's not used
  kape::Pheromones ph_anthill(kape::Pheromones::Type::TO_ANTHILL, 1.);
  kape::Pheromones ph_food(kape::Pheromones::Type::TO_FOOD, 1.);
  SUBCASE("Testing getPheromonesType function")
  {
    CHECK(ph_anthill.getPheromonesType() == kape::Pheromones::Type::TO_ANTHILL);
    CHECK(ph_food.getPheromonesType() == kape::Pheromones::Type::TO_FOOD);
  }
  SUBCASE("Testing getNumberOfPheromones and addPheromonePrticle functions")
  {
    CHECK(ph_anthill.getNumberOfPheromones() == 0);
    CHECK(ph_food.getNumberOfPheromones() == 0);
    for (double d{0.}; d != 10.; ++d) {
      ph_anthill.addPheromoneParticle(kape::Vector2d{d, d}, 100);
    }
    for (double d{0.}; d != 15.; ++d) {
      ph_food.addPheromoneParticle(kape::Vector2d{d, 0.}, 100);
    }
    CHECK(ph_anthill.getNumberOfPheromones() == 10);
    CHECK(ph_food.getNumberOfPheromones() == 15);
  }
  SUBCASE("Testing getPheromonesIntensityInCircle function")
  {
    for (double d{0.}; d != 10.; ++d) {
      ph_anthill.addPheromoneParticle(kape::Vector2d{d, d}, 100);
    }
    for (double d{0.}; d != 15.; ++d) {
      ph_food.addPheromoneParticle(kape::Vector2d{d, 0.}, 100);
    }
    CHECK(ph_anthill.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{3., 2.}, 1.5})
          == 200);
    CHECK(ph_anthill.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{10., 9.}, 1.})
          == 100);
    CHECK(ph_anthill.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{10., 4.}, 2.5})
          == 0);
    CHECK(ph_food.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{6., 1.}, 3.})
          == 500);
  }
  SUBCASE("Testing updateParticlesEvaporation function")
  {
    for (int i{0}; i != 10; ++i) {
      ph_anthill.addPheromoneParticle(kape::Vector2d{0, 0}, 100);
    }
    ph_anthill.addPheromoneParticle(kape::Vector2d{0., 0.}, 1);

    for (int i{0}; i != 15; ++i) {
      ph_food.addPheromoneParticle(kape::Vector2d{0, 0}, 100);
    }
    ph_food.addPheromoneParticle(kape::Vector2d{0., 0.}, 1);

    ph_anthill.updateParticlesEvaporation(0.);
    ph_food.updateParticlesEvaporation(0.);
    CHECK(ph_anthill.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{0., 0.}, 1.})
          == 1001);
    CHECK(ph_food.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{0., 0.}, 1.})
          == 1501);

    ph_anthill.updateParticlesEvaporation(
        kape::Pheromones::PERIOD_BETWEEN_EVAPORATION_UPDATE_ / 10.);
    ph_food.updateParticlesEvaporation(
        kape::Pheromones::PERIOD_BETWEEN_EVAPORATION_UPDATE_ / 10.);
    CHECK(ph_anthill.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{0., 0.}, 1.})
          == 1001);
    CHECK(ph_food.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{0., 0.}, 1.})
          == 1501);

    ph_anthill.updateParticlesEvaporation(
        kape::Pheromones::PERIOD_BETWEEN_EVAPORATION_UPDATE_);
    ph_food.updateParticlesEvaporation(
        kape::Pheromones::PERIOD_BETWEEN_EVAPORATION_UPDATE_);
    CHECK(ph_anthill.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{0., 0.}, 1.})
          == (1001 * (1 - 0.005)));
    CHECK(ph_anthill.getNumberOfPheromones() == 11);
    CHECK(ph_food.getPheromonesIntensityInCircle(
              kape::Circle{kape::Vector2d{0., 0.}, 1.})
          == (1501 * (1 - 0.005)));
    CHECK(ph_food.getNumberOfPheromones() == 16);

    for (int i{0}; i != 1000; ++i) {
      ph_anthill.updateParticlesEvaporation(
          kape::Pheromones::PERIOD_BETWEEN_EVAPORATION_UPDATE_ * i);
      ph_food.updateParticlesEvaporation(
          kape::Pheromones::PERIOD_BETWEEN_EVAPORATION_UPDATE_ * i);
    }
    CHECK(ph_anthill.getNumberOfPheromones() == 10);
    CHECK(ph_food.getNumberOfPheromones() == 15);

    CHECK_THROWS(ph_anthill.updateParticlesEvaporation(-0.1));
    CHECK_THROWS(ph_food.updateParticlesEvaporation(-0.3));
  }
  SUBCASE("Testing const iterators begin && end")
  {
    for (int i{0}; i != 10; ++i) {
      ph_anthill.addPheromoneParticle(kape::Vector2d{0, 0}, 100);
    }
    for (int i{0}; i != 15; ++i) {
      ph_food.addPheromoneParticle(kape::Vector2d{0, 0}, 100);
    }
    int number_of_pheromones{0};
    for (auto it = ph_anthill.begin(), end = ph_anthill.end(); it != end;
         ++it) {
      ++number_of_pheromones;
    }
    CHECK(number_of_pheromones == 10);
    for (auto it = ph_food.begin(), end = ph_food.end(); it != end; ++it) {
      ++number_of_pheromones;
    }
    CHECK(number_of_pheromones == 25);
  }
}

TEST_CASE("Testing Anthill class")
{
  kape::Anthill anthill1{};
  kape::Anthill anthill2{kape::Vector2d{10.5, 3.4}, 2.5, 5};
  SUBCASE("Testing getCircle function")
  {
    CHECK(anthill1.getCircle().getCircleCenter().x == doctest::Approx(0.));
    CHECK(anthill1.getCircle().getCircleCenter().y == doctest::Approx(0.));
    CHECK(anthill1.getCircle().getCircleRadius() == doctest::Approx(1.));
    CHECK(anthill2.getCircle().getCircleCenter().x == doctest::Approx(10.5));
    CHECK(anthill2.getCircle().getCircleCenter().y == doctest::Approx(3.4));
    CHECK(anthill2.getCircle().getCircleRadius() == doctest::Approx(2.5));
  }
  SUBCASE("Testing getCenter function")
  {
    CHECK(anthill1.getCenter().x == doctest::Approx(0.));
    CHECK(anthill1.getCenter().y == doctest::Approx(0.));
    CHECK(anthill2.getCenter().x == doctest::Approx(10.5));
    CHECK(anthill2.getCenter().y == doctest::Approx(3.4));
  }
  SUBCASE("Testing getRadius function")
  {
    CHECK(anthill1.getRadius() == doctest::Approx(1.));
    CHECK(anthill2.getRadius() == doctest::Approx(2.5));
  }
  SUBCASE("Testing getFoodCounter function")
  {
    CHECK(anthill1.getFoodCounter() == 0);
    CHECK(anthill2.getFoodCounter() == 5);
    anthill1.addFood(12);
    anthill2.addFood(465);
    CHECK(anthill1.getFoodCounter() == 12);
    CHECK(anthill2.getFoodCounter() == 470);
  }
  SUBCASE("Testing isInside function")
  {
    CHECK(anthill1.isInside(kape::Vector2d{0.5, 0.2}) == true);
    CHECK(anthill1.isInside(kape::Vector2d{1., 0.}) == true);
    CHECK(anthill1.isInside(kape::Vector2d{0.9, 0.9}) == false);
    CHECK(anthill2.isInside(kape::Vector2d{11., 2.}) == true);
    CHECK(anthill2.isInside(kape::Vector2d{11., 0.9}) == false);
    CHECK(anthill2.isInside(kape::Vector2d{10.5, 0.9}) == true);
  }
  SUBCASE("Testing addFood function")
  {
    anthill1.addFood(1);
    CHECK(anthill1.getFoodCounter() == 1);
    anthill2.addFood(43);
    CHECK(anthill2.getFoodCounter() == 48);
    CHECK_THROWS(anthill1.addFood(-1));
    CHECK_THROWS(anthill2.addFood(-8));
  }
}