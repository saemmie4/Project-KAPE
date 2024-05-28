#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "environment.hpp"
#include "doctest.h"

TEST_CASE("Testing Obstacles class")
{
  SUBCASE("Testing addObstacle function")
  {
    // testare entrambi i modi
  }
  SUBCASE("Testing anyObstacleInCircle function")
  {}
  // loadFromFile, saveToFile, both iterators ?
}

TEST_CASE("Testing FoodParticle class")
{
  SUBCASE("Testing getPosition function")
  {}
  // should we test the constructor as well?
}

TEST_CASE("Testing PheromoneParticle class")
{
  SUBCASE("Testing getPosition function")
  {}
  SUBCASE("Testing getIntensity function")
  {}
  SUBCASE("Testing decreaseIntensity function")
  {}
  SUBCASE("Testing hasEvaporated function")
  {}
  // should we test the constructor as well?
}

TEST_CASE("Testing Food class")
{
  //?
}

TEST_CASE("Testing Iterator class")
{
  SUBCASE("Testing operator++")
  {}
  SUBCASE("Testing operator*")
  {}
  SUBCASE("Testing operator==")
  {}
  SUBCASE("Testing operator!=")
  {}
  // should we test the constructor as well?
}

TEST_CASE("Testing Pheromones class")
{
  SUBCASE("Testing getPheromonesIntensityInCircle function")
  {}
  SUBCASE("Testing getPheromonesType function")
  {}
  SUBCASE("Testing getNumberOfPheromones function")
  {}
  SUBCASE("Testing addPheromoneParticle function")
  {}
  SUBCASE("Testing updateParticlesEvaporation function")
  {}
}

TEST_CASE("Testing Anthill class")
{
  SUBCASE("Testing getCircle function")
  {}
  SUBCASE("Testing getCenter function")
  {}
  SUBCASE("Testing getRadius function")
  {}
  SUBCASE("Testing getFoodCounter function")
  {}
  SUBCASE("Testing isInside function")
  {}
  SUBCASE("Testing addFood function")
  {}
  // loadFromFile, saveToFile ?
}