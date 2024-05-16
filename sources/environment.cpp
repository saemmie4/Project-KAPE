#include "environment.hpp"
#include <algorithm> //for find_if
#include <cmath>
#include <numeric>   //for accumulate
#include <stdexcept> //invalid_argument

namespace kape {

// FoodParticle class Implementation--------------------------
FoodParticle::FoodParticle(Vector2d const& position)
    : position_{position}
{}

FoodParticle::FoodParticle(FoodParticle const& food_particle)
    : FoodParticle{food_particle.getPosition()}
{}

Vector2d FoodParticle::getPosition() const
{
  return position_;
}

// PheromoneParticle class Implementation--------------------------
PheromoneParticle::PheromoneParticle(Vector2d const& position, int intensity)
    : position_{position}
    , intensity_{intensity}
{
  if (intensity_ < 0 || intensity_ > 100)
    throw std::invalid_argument{
        "The pheromones intensity must be between 0 and 100"};
}

PheromoneParticle::PheromoneParticle(
    PheromoneParticle const& pheromone_particle)
    : PheromoneParticle{pheromone_particle.getPosition(),
                        pheromone_particle.getIntensity()}
{}

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

// Food Class implementation -----------------------------------
Food::Food()
{}

void Food::addFoodParticle(Vector2d const& position)
{
  food_vec_.push_back(FoodParticle{position});
}

void Food::addFoodParticle(FoodParticle const& food_particle)
{
  food_vec_.push_back(food_particle);
}

bool Food::isThereFoodLeft() const
{
  return !food_vec_.empty();
}

bool Food::removeOneFoodParticleInCircle(Circle const& circle)
{
  auto food_particle_it{
      std::find_if(food_vec_.begin(), food_vec_.end(),
                   [&circle](FoodParticle const& food_particle) {
                     return circle.isInside(food_particle.getPosition());
                   })};

  if (food_particle_it == food_vec_.end()) {
    return false;
  }

  food_vec_.erase(food_particle_it);
  return true;
}

// Pheromones class implementation ------------------------------
Pheromones::Pheromones(Type type)
    : type_{type}
{}

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

Pheromones::Type Pheromones::getPheromonesType() const
{
  return type_;
}

void Pheromones::addPheromoneParticle(Vector2d const& position, int intensity)
{
  pheromones_vec_.push_back(PheromoneParticle{position, intensity});
}

void Pheromones::addPheromoneParticle(PheromoneParticle const& particle)
{
  pheromones_vec_.push_back(particle);
}

void Pheromones::updateParticlesEvaporation(int amount)
{
  // the check if amount < 0 is done in decreaseIntensity
  for (auto& pheromone : pheromones_vec_) {
    pheromone.decreaseIntensity(amount);
  }
}

// implementation of class Anthill
Anthill::Anthill(Vector2d center, double radius, int food_counter)
    : circle_{center, radius}
    , food_counter_{food_counter}
{
  if (food_counter_ < 0) {
    throw std::invalid_argument{"the food counter can't be negative"};
  }
}
Anthill::Anthill(Circle const& circle, int food_counter = 0)
{

}

Circle Anthill::getCircle() const
{

}



Vector2d Anthill::getPosition() const
{
  return position_;
}
double Anthill::getRadius() const
{
  return radius_;
}
int Anthill::getFoodCounter() const
{
  return food_counter_;
}
bool Anthill::isInside(Vector2d position) const
{
  return 
}

void Anthill::addFood(int amount)
{
  if (amount < 0) {
    throw std::invalid_argument{"the amount of food added can't be negative"};
  }

  food_counter_ += amount;
}

// implementation of class Obstacles-----------------------------------

Obstacles::Obstacles()
{}

void Obstacles::addObstacle(Vector2d const& top_left_corner, double width,
                            double height)
{
  obstacles_vec_.push_back(Rectangle{top_left_corner, width, height});
}
void Obstacles::addObstacle(Rectangle const& obstacle)
{
  obstacles_vec_.push_back(obstacle);
}

bool Obstacles::anyObstaclesInCircle(Circle const& circle) const
{
  return std::any_of(obstacles_vec_.begin(), obstacles_vec_.end(),
                     [&circle](Rectangle const& obstacle) {
                       return doShapesIntersect(circle, obstacle);
                     });
}

} // namespace kape