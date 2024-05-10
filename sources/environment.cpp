#include "environment.hpp"
#include <stdexcept>

namespace kape {

FoodParticle::FoodParticle(Vector2d const& position)
    : position_{position}
{}

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
  if (amount < 0)
    throw std::invalid_argument{"The amount must be a positive number"};
    intensity_ += -(amount);
}

bool PheromoneParticle::hasEvaporated() const
{
  if (intensity_ == 0)
    return true;
}

Pheromones::Pheromones(PheromoneType type)
    : type_{type}
{}

void Pheromones::addPheromoneParticle(Vector2d const& position, int intensity)
{
    PheromoneParticle particle(position, intensity);
    pheromones_vec_.push_back(particle);
}

void Pheromones::addPheromoneParticle(PheromoneParticle const& particle) {
    pheromones_vec_.push_back(particle);
}

void Pheromones::updateParticlesEvaporation(int amount) {
    
}
} // namespace kape