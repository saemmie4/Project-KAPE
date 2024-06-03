#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "geometry.hpp" //for Vector2d
#include <random>
#include <stdexcept>
#include <vector>

#include <iterator>
// TODO:
//  - check if adding things to a vector can cause exceptions
//  - Anthill::loadFromFile() should check if it intersects any obstacles
namespace kape {
class Obstacles
{
  std::vector<Rectangle> obstacles_vec_;

 public:
  inline static std::string const DEFAULT_FILEPATH_{
      "./assets/obstacles/obstacles.dat"};

  explicit Obstacles();
  void addObstacle(Vector2d const& top_left_corner, double width,
                   double height);
  void addObstacle(Rectangle const& obstacle);
  bool anyObstaclesInCircle(Circle const& circle) const;

  bool loadFromFile(std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_);

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
  // below this threshold the pheromone is considered to have evaporated
  inline static double const MIN_PHEROMONE_INTENSITY{0.02};

  Vector2d position_;
  double intensity_;

 public:
  // may throw std::invalid_argument if intensity <= 0.
  PheromoneParticle(Vector2d const& position, double intensity);
  // PheromoneParticle(PheromoneParticle const& pheromone_particle);
  Vector2d const& getPosition() const;
  double getIntensity() const;

  // may throw std::invalid_argument if decrease_percentage_amount isn't in [0,
  // 1)
  void decreaseIntensity(double decrease_percentage_amount = 0.005);
  // returns true if the Pheromone's intensity is <= MIN_PHEROMONE_INTENSITY
  bool hasEvaporated() const;
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
    // may throw std::invalid_argument if the circle intersects with any of the
    // obstacles
    explicit CircleWithFood(Circle const& circle,
                            std::size_t number_of_food_particles,
                            Obstacles const& obs,
                            std::default_random_engine& engine);
    Circle const& getCircle() const;
    std::size_t getNumberOfFoodParticles() const;
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
  inline static std::string const DEFAULT_FILEPATH_{"./assets/food/food.dat"};
  explicit Food(unsigned int seed = 11u);
  std::size_t getNumberOfFoodParticles() const;

  // returns:
  //  - true if it generated the food_particles
  //  - false if it didn't generate any particle, i.e. the circle intersects at
  //    least in part one obstacle
  //
  // iterators of class Food::Iterator are invalidated
  // (in particular, an iterator that was == end() may now be != end())
  bool generateFoodInCircle(Circle const& circle,
                            std::size_t number_of_food_particles,
                            Obstacles const& obstacles);
  bool isThereFoodLeft() const;
  // returns true if there was food in the circle (therefore if has also been
  // removed) returns false if there wasn't any food in the circle (therefore
  // nothing has been removed)
  // iterators of class Food::Iterator are invalidated if true
  bool removeOneFoodParticleInCircle(Circle const& circle);

  bool loadFromFile(Obstacles const& obstacles,
                    std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_);

  class Iterator
  {
   private:
    std::vector<FoodParticle>::const_iterator food_particle_it_;
    std::vector<CircleWithFood>::const_iterator circle_with_food_it_;
    std::vector<CircleWithFood>::const_iterator const circle_with_food_back_it_;

   public:
    explicit Iterator(
        std::vector<FoodParticle>::const_iterator const& food_particle_it,
        std::vector<CircleWithFood>::const_iterator const& circle_with_food_it,
        std::vector<CircleWithFood>::const_iterator const&
            circle_with_food_back_it);
    Iterator& operator++(); // prefix ++
    FoodParticle const& operator*() const;

    friend bool operator==(Iterator const& lhs, Iterator const& rhs);
    friend bool operator!=(Iterator const& lhs, Iterator const& rhs);
  };

  Iterator begin() const;
  Iterator end() const;
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
  // every PERIOD_BETWEEN_EVAPORATION_UPDATE_ each pheromone particle loses some intesity
  static double constexpr PERIOD_BETWEEN_EVAPORATION_UPDATE_{.5};

  std::vector<PheromoneParticle> pheromones_vec_;
  const Type type_;
  std::default_random_engine random_engine_;
  double time_since_last_evaporation_;

 public:
  explicit Pheromones(Type type, unsigned int seed = 31415u);
  double getPheromonesIntensityInCircle(Circle const& circle) const;
  //returns end() if there were no pheromones in the circle
  std::vector<PheromoneParticle>::const_iterator getRandomMaxPheromoneParticleInCircle(Circle const& circle);
  Pheromones::Type getPheromonesType() const;
  std::size_t getNumberOfPheromones() const;
  // may throw std::invalid_argument if intensity is <= 0.
  void addPheromoneParticle(Vector2d const& position, double intensity);
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
  inline static std::string const DEFAULT_FILEPATH_{
      "./assets/anthill/anthill.dat"};

  // may throw std::invalid_argument if radius<=0 or food_counter < 0
  explicit Anthill(Vector2d center = Vector2d{0., 0.}, double radius = 1.,
                   int food_counter = 0);
  explicit Anthill(Circle const& circle, int food_counter = 0);
  Circle const& getCircle() const;
  Vector2d const& getCenter() const;
  double getRadius() const;
  int getFoodCounter() const;
  bool isInside(Vector2d const& position) const;
  // may throw std::invalid_argument if amount < 0
  void addFood(int amount = 1);

  // if the function fails it leaves anthill as it was before the call
  bool loadFromFile(std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_);
};

} // namespace kape

#endif