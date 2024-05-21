#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "geometry.hpp" //for Vector2d
#include <random>
#include <stdexcept>
#include <vector>

#include <iterator>
// TODO:
//  - Add Obstacles class
//  - Food class
//  - check if adding things to a vector can cause exceptions

namespace kape {
class Obstacles
{
  std::vector<Rectangle> obstacles_vec_;

 public:
  explicit Obstacles();
  void addObstacle(Vector2d const& top_left_corner, double width,
                   double height);
  void addObstacle(Rectangle const& obstacle);
  bool anyObstaclesInCircle(Circle const& circle) const;

  std::vector<Rectangle>::const_iterator begin() const;
  std::vector<Rectangle>::const_iterator end() const;
};

class FoodParticle
{
 private:
  Vector2d position_;

 public:
  explicit FoodParticle(Vector2d const& position);
  // explicit FoodParticle(FoodParticle const& food_particle);
  Vector2d const& getPosition() const;

  //
  // FoodParticle& operator=(FoodParticle const& rhs);
};

class PheromoneParticle
{
 private:
  Vector2d position_;
  int intensity_;

 public:
  // may throw std::invalid_argument if intensity isn't in [0,100]
  PheromoneParticle(Vector2d const& position, int intensity = 100);
  // PheromoneParticle(PheromoneParticle const& pheromone_particle);
  Vector2d const& getPosition() const;
  // returns the intensity as a number in the interval [0, 100]
  int getIntensity() const;

  // may throw std::invalid_argument if amount < 0
  void decreaseIntensity(int amount = 1);
  // returns true if the Pheromone's intensity is 0
  bool hasEvaporated() const;
  // PheromoneParticle& operator=(PheromoneParticle const& rhs);
};

class Food
{
 private:
  class CircleWithFood
  {
   private:
    Circle circle_;
    std::vector<FoodParticle> food_vec_;

   public:
    // may throw std::invalid_argument if number_of_food_particles < 0 or if
    // the circle intersects with any of the obstacles
    explicit CircleWithFood(Circle const& circle, int number_of_food_particles,
                            Obstacles const& obs,
                            std::default_random_engine& engine);
    Circle const& getCircle() const;
    bool removeOneFoodParticleInCircle(Circle const& circle);
    bool isThereFoodLeft() const;

    // add auto?
    std::vector<FoodParticle>::const_iterator begin() const;
    std::vector<FoodParticle>::const_iterator end() const;
  };

  std::vector<CircleWithFood> circles_with_food_vec_;
  std::default_random_engine engine_;

  // void addFoodParticle(Vector2d const& position);
  // void addFoodParticle(FoodParticle const& food_particle);

 public:
  explicit Food(unsigned int seed = 11u);
  // returns:
  //  - true if it generated the food_particles
  //  - false if it didn't generate any particle, i.e. the circle intersects at
  //    least in part one obstacle
  //
  // may throw std::invalid_argument if number_of_particles<0
  // iterators of class Food::Iterator are invalidated
  // (in particular, an iterator that was == end() may now be != end())
  bool generateFoodInCircle(Circle const& circle, int number_of_food_particles,
                            Obstacles const& obstacles);
  bool isThereFoodLeft() const;
  // returns true if there was food in the circle (therefore if has also been
  // removed) returns false if there wasn't any food in the circle (therefore
  // nothing has been removed)
  // iterators of class Food::Iterator are invalidated if true
  bool removeOneFoodParticleInCircle(Circle const& circle);

  // add auto?
  std::vector<FoodParticle>::const_iterator
  next(std::vector<FoodParticle>::const_iterator food_it) const;

  class iterator
  {
   private:
    std::vector<FoodParticle>::const_iterator it_;
    Food const& food_container_;

    //  protected:
    //   Food const& getFoodVectorIterator();
    //   Food const& getContainer();

   public:
    explicit iterator(std::vector<FoodParticle>::const_iterator it,
                      Food const& food_container);
    iterator& operator++(); // prefix ++
    FoodParticle const& operator*() const;

    friend bool operator==(iterator const& lhs, iterator const& rhs);
    friend bool operator!=(iterator const& lhs, iterator const& rhs);
  };

  iterator begin() const;
  iterator end() const;
  // std::vector<FoodParticle>::const_iterator begin() const;
  // std::vector<FoodParticle>::const_iterator end() const;
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
  // every PERIOD_BETWEEN_EVAPORATION_UPDATE_ each pheromone particle loses 5
  // intensity levels
  static double constexpr PERIOD_BETWEEN_EVAPORATION_UPDATE_{1.};

  std::vector<PheromoneParticle> pheromones_vec_;
  const Type type_;
  double time_since_last_evaporation_;

 public:
  explicit Pheromones(Type type);
  int getPheromonesIntensityInCircle(Circle const& circle) const;
  Pheromones::Type getPheromonesType() const;
  std::size_t getNumberOfPheromones() const;
  // may throw std::invalid_argument if intensity isn't in [0,100]
  void addPheromoneParticle(Vector2d const& position, int intensity = 100);
  void addPheromoneParticle(PheromoneParticle const& particle);
  // may throw std::invalid_argument if delta_t<0.
  void updateParticlesEvaporation(double delta_t = 0.01);

  std::vector<PheromoneParticle>::const_iterator begin() const;
  std::vector<PheromoneParticle>::const_iterator end() const;
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
  Circle const& getCircle() const;
  Vector2d const& getCenter() const;
  double getRadius() const;
  int getFoodCounter() const;
  bool isInside(Vector2d const& position) const;
  // may throw std::invalid_argument if amount < 0
  void addFood(int amount = 1);
};

} // namespace kape

#endif