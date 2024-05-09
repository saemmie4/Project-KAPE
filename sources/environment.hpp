#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "geometry.hpp" //for Vector2d
#include <vector>
// TODO:
//  everything

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
  PheromoneParticle(Vector2d const& position, int intensity_ = 100);
  Vector2d getPosition() const;
  //returns the intensity as a number in the interval [0, 100]
  int getIntensity() const;
  

  void decreaseIntensity(int amount = 5);
  //returns true if the Pheromone's intensity is 0
  bool hasEvaporated() const;
};

class Pheromones{
  public:
    enum class PheromoneType {TO_FOOD, TO_COLONY};
  
  private:
  std::vector<PheromoneParticle> pheromones_vec_;
  const PheromoneType type_;

  public: 
  explicit Pheromones(PheromoneType type);
  void addPheromoneParticle(Vector2d const& position, int intensity = 100);
  void addPheromoneParticle(PheromoneParticle const& particle);
  void updateParticlesIntensity(int amount = 5);
};
//  Pheromones redPheromones(Pheromones::PheromoneType::RED);

class Environment
{};
} // namespace kape

#endif