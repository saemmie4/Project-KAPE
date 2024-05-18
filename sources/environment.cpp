#include "environment.hpp"
#include "geometry.hpp"
#include <algorithm> //for find_if and remove_if any_of
#include <cmath>
#include <numeric>   //for accumulate
#include <stdexcept> //invalid_argument
#include <vector>

namespace kape {

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

std::vector<Rectangle>::const_iterator Obstacles::begin() const
{
  return obstacles_vec_.cbegin();
}
std::vector<Rectangle>::const_iterator Obstacles::end() const
{
  return obstacles_vec_.cend();
}

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
Food::Food(long unsigned int seed)
    : food_vec_{}
    , engine_{seed}
{}

void Food::addFoodParticle(Vector2d const& position)
{
  food_vec_.push_back(FoodParticle{position});
}

void Food::addFoodParticle(FoodParticle const& food_particle)
{
  food_vec_.push_back(food_particle);
}

// returns:
//  - true if it generated the food_particles
//  - false if it didn't generate any particle, i.e. the circle intersects at
//    least in part one obstacle
bool Food::generateFoodInCircle(Circle const& circle,
                                int number_of_food_particles,
                                Obstacles const& obstacles)
{
  // if the circle intersects any obstacles
  if (std::any_of(obstacles.begin(), obstacles.end(),
                  [&circle](Rectangle const& rectangle) {
                    return doShapesIntersect(circle, rectangle);
                  })) {
    return false;
  }

  std::uniform_real_distribution angle_distribution{0., 2 * PI};
  // sigma = radius/3. makes the probability of having a generated distance from
  // the center = 99.7%.
  std::normal_distribution center_distance_distribution{
      0., circle.getCircleRadius() / 3.};

  std::generate_n(
      std::back_inserter(food_vec_), number_of_food_particles,
      [&circle, &center_distance_distribution, &angle_distribution, this]() {
        double angle{angle_distribution(engine_)};
        double center_distance{std::abs(center_distance_distribution(engine_))};
        if (center_distance > circle.getCircleRadius()) {
          center_distance = circle.getCircleRadius();
        }

        Vector2d position{rotate({0., 1.}, angle)};
        position *= center_distance;
        position += circle.getCircleCenter();
        return FoodParticle{position};
      });

  return true;
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

std::vector<FoodParticle>::const_iterator Food::begin() const
{
  return food_vec_.cbegin();
}
std::vector<FoodParticle>::const_iterator Food::end() const
{
  return food_vec_.cend();
}

// Pheromones class implementation ------------------------------
Pheromones::Pheromones(Type type)
    : pheromones_vec_{}
    , type_{type}
    , time_since_last_evaporation_{0.}
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
std::size_t Pheromones::getNumberOfPheromones() const
{
  return pheromones_vec_.size();
}

void Pheromones::addPheromoneParticle(Vector2d const& position, int intensity)
{
  pheromones_vec_.push_back(PheromoneParticle{position, intensity});
}

void Pheromones::addPheromoneParticle(PheromoneParticle const& particle)
{
  pheromones_vec_.push_back(particle);
}

// may throw std::invalid_argument if delta_t<0.
void Pheromones::updateParticlesEvaporation(double delta_t)
{
  if (delta_t < 0.) {
    throw std::invalid_argument{"delta_t can't be negative"};
  }

  time_since_last_evaporation_ += delta_t;
  if (time_since_last_evaporation_ < PERIOD_BETWEEN_EVAPORATION_UPDATE_) {
    return;
  }
  time_since_last_evaporation_ -= PERIOD_BETWEEN_EVAPORATION_UPDATE_;

  for (auto& pheromone : pheromones_vec_) {
    pheromone.decreaseIntensity();
  }

  // remove phermones that have evaporated
  pheromones_vec_.erase(std::remove_if(pheromones_vec_.begin(),
                                       pheromones_vec_.end(),
                                       [](PheromoneParticle const& particle) {
                                         return particle.hasEvaporated();
                                       }),
                        pheromones_vec_.end());
}

std::vector<PheromoneParticle>::const_iterator Pheromones::begin() const
{
  return pheromones_vec_.cbegin();
}
std::vector<PheromoneParticle>::const_iterator Pheromones::end() const
{
  return pheromones_vec_.cend();
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
Anthill::Anthill(Circle const& circle, int food_counter)
    : Anthill{circle.getCircleCenter(), circle.getCircleRadius(), food_counter}
{}

Circle Anthill::getCircle() const
{
  return circle_;
}

Vector2d Anthill::getCenter() const
{
  return circle_.getCircleCenter();
}
double Anthill::getRadius() const
{
  return circle_.getCircleRadius();
}
int Anthill::getFoodCounter() const
{
  return food_counter_;
}
bool Anthill::isInside(Vector2d position) const
{
  return circle_.isInside(position);
}

void Anthill::addFood(int amount)
{
  if (amount < 0) {
    throw std::invalid_argument{"the amount of food added can't be negative"};
  }

  food_counter_ += amount;
}

} // namespace kape