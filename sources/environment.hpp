#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "geometry.hpp" //for Vector2d
#include <stdexcept>
#include <vector>
// TODO:
//  - Add Obstacles class
//  - Food class
//  - check if adding things to a vector can cause exceptions

namespace kape {
class FoodParticle
{
 private:
  Vector2d position_;

 public:
  FoodParticle(Vector2d const& position);
};

class PheromoneParticle
{
 private:
  Vector2d position_;
  int intensity_;

 public:
  // may throw std::invalid_argument if intensity isn't in [0,100]
  PheromoneParticle(Vector2d const& position, int intensity = 100);
  Vector2d getPosition() const;
  // returns the intensity as a number in the interval [0, 100]
  int getIntensity() const;

  // may throw std::invalid_argument if amount < 0
  void decreaseIntensity(int amount = 5);
  // returns true if the Pheromone's intensity is 0
  bool hasEvaporated() const;
};

class Food
{
 private:
  std::vector<FoodParticle> food_vec_;

 public:
  Food();
  void addFoodParticle(Vector2d const& position);
  void addFoodParticle(FoodParticle const& food_particle);
  bool isThereFoodLeft();
  // returns true if there was food in the circle (therefore if has also been
  // removed) returns false if there wasn't any food in the circle (therefore
  // nothing has been removed)
  bool removeOneFoodParticleInCircle(Circle const& circle);
};

class Pheromones
{
 public:
  enum class PheromoneType
  {
    TO_FOOD,
    TO_ANTHILL
  };

 private:
  std::vector<PheromoneParticle> pheromones_vec_;
  const PheromoneType type_;

 public:
  explicit Pheromones(PheromoneType type);
  // may throw std::invalid_argument if intensity isn't in [0,100]
  void addPheromoneParticle(Vector2d const& position, int intensity = 100);
  void addPheromoneParticle(PheromoneParticle const& particle);
  // may throw std::invalid_argument if amount < 0
  void updateParticlesEvaporation(int amount = 5);
  int getPheromonesIntensityInCircle(Circle const& circle) const;
};


class Obstacles
{
  std::vector<Rectangle> obstacles_vec_;

 public:
  Obstacles();
  void addObstacle(Vector2d const& top_left_corner, double width, double height);
  void addObstacle(Rectangle const& obstacle);
  bool anyObstaclesInCircle(Circle const& circle) const;
};
} // namespace kape

#endif