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
  explicit FoodParticle(Vector2d const& position);
  explicit FoodParticle(FoodParticle const& food_particle);
  Vector2d getPosition() const;
};

class PheromoneParticle
{
 private:
  Vector2d position_;
  int intensity_;

 public:
  // may throw std::invalid_argument if intensity isn't in [0,100]
  PheromoneParticle(Vector2d const& position, int intensity = 100);
  PheromoneParticle(PheromoneParticle const& pheromone_particle);
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
  explicit Food();
  void addFoodParticle(Vector2d const& position);
  void addFoodParticle(FoodParticle const& food_particle);
  bool isThereFoodLeft() const;
  // returns true if there was food in the circle (therefore if has also been
  // removed) returns false if there wasn't any food in the circle (therefore
  // nothing has been removed)
  bool removeOneFoodParticleInCircle(Circle const& circle);
};

class Pheromones
{
 public:
  enum class Type
  {
    TO_FOOD,
    TO_ANTHILL
  };

 private:
  std::vector<PheromoneParticle> pheromones_vec_;
  const Type type_;

 public:
  explicit Pheromones(Type type);
  int getPheromonesIntensityInCircle(Circle const& circle) const;
  Pheromones::Type getPheromonesType() const;
  // may throw std::invalid_argument if intensity isn't in [0,100]
  void addPheromoneParticle(Vector2d const& position, int intensity = 100);
  void addPheromoneParticle(PheromoneParticle const& particle);
  // may throw std::invalid_argument if amount < 0
  void updateParticlesEvaporation(int amount = 5);
};

class Anthill
{
 private:
  Circle circle_;
  int food_counter_;

 public:
  // may throw std::invalid_argument if radius<=0 or food_counter < 0
  explicit Anthill(Vector2d center, double radius, int food_counter = 0);
  explicit Anthill(Circle const& circle, int food_counter = 0);
  Circle getCircle() const;
  Vector2d getCenter() const;
  double getRadius() const;
  int getFoodCounter() const;
  bool isInside(Vector2d position) const;
  // may throw std::invalid_argument if amount < 0
  void addFood(int amount = 1);
};

class Obstacles
{
  std::vector<Rectangle> obstacles_vec_;

 public:
  explicit Obstacles();
  void addObstacle(Vector2d const& top_left_corner, double width,
                   double height);
  void addObstacle(Rectangle const& obstacle);
  bool anyObstaclesInCircle(Circle const& circle) const;
};
} // namespace kape

#endif