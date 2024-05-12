#include "environment.hpp"
#include <cmath>
#include <numeric> //for accumulate
#include <stdexcept>

namespace kape {

FoodParticle::FoodParticle(Vector2d const& position)
    : position_{position}
{}

void Food::addFoodParticle(Vector2d const& position) {
  FoodParticle particle{position};
  food_vec_.push_back(particle);
}

void Food::addFoodParticle(FoodParticle const& food_particle) {
  food_vec_.push_back(food_particle);
}

PheromoneParticle::PheromoneParticle(Vector2d const& position, int intensity)
    : position_{position}
    , intensity_{intensity}
{
  if (intensity_ < 0 || intensity_ > 100)
    throw std::invalid_argument{
        "The pheromones intensity must be between 0 and 100"};
}

Vector2d PheromoneParticle::getPosition() const
{
  return position_;
};

int PheromoneParticle::getIntensity() const
{
  return intensity_;
}

void PheromoneParticle::decreaseIntensity(int amount)
{
  if (amount < 0) {
    throw std::invalid_argument{"The amount must be a positive number"};
  }

  intensity_ -= amount;
  if (intensity_ < 0) {
    intensity_ = 0;
  }
}

bool PheromoneParticle::hasEvaporated() const
{
  return intensity_ == 0;
}

Pheromones::Pheromones(PheromoneType type)
    : type_{type}
{}

void Pheromones::addPheromoneParticle(Vector2d const& position, int intensity)
{
  PheromoneParticle particle{position, intensity};
  pheromones_vec_.push_back(particle);
}

void Pheromones::addPheromoneParticle(PheromoneParticle const& particle)
{
  pheromones_vec_.push_back(particle);
}

void Pheromones::updateParticlesEvaporation(int amount)
{
  for (auto& pheromone : pheromones_vec_) {
    pheromone.decreaseIntensity(amount);
  }
}

// add function which returns the difference of two Vector2d to use in lambda
int Pheromones::getPheromonesIntensityInCircle(Circle const& circle) const
{
  return std::accumulate(
      pheromones_vec_.begin(), pheromones_vec_.end(), 0,
      [&circle](int sum, PheromoneParticle const& pheromone) {
        return sum
             + (circle.isInside(pheromone.getPosition())
                    ? pheromone.getIntensity()
                    : 0);
      });
}
} // namespace kape