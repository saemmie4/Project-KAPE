#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "geometry.hpp" //for Vector2d
#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace kape {

class Obstacles
{
  std::vector<Rectangle> obstacles_vec_;

 public:
  inline static std::string const DEFAULT_FILEPATH_{
      "./assets/simulations/map_1/obstacles/obstacles.dat"};

  explicit Obstacles();
  std::size_t getNumberOfObstacles() const;
  void addObstacle(Vector2d const& top_left_corner, double width,
                   double height);
  void addObstacle(Rectangle const& obstacle);
  bool anyObstaclesInCircle(Circle const& circle) const;

  bool loadFromFile(std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_) const;

  std::vector<Rectangle>::const_iterator begin() const;
  std::vector<Rectangle>::const_iterator end() const;
};

class FoodParticle
{
 private:
  Vector2d position_;

 public:
  explicit FoodParticle(Vector2d const& position);
  Vector2d const& getPosition() const;
};

class PheromoneParticle
{
 private:
  Vector2d position_;
  double intensity_;

 public:
  // may throw std::invalid_argument if intensity <= 0.
  PheromoneParticle(Vector2d const& position, double intensity);
  Vector2d const& getPosition() const;
  double getIntensity() const;

  // may throw std::invalid_argument if decrease_percentage_amount isn't in [0,
  // 1)
  void decreaseIntensity(double decrease_percentage_amount,
                         double min_pheromone_intensity);
  // returns true if the Pheromone's intensity is <= MIN_PHEROMONE_INTENSITY
  bool hasEvaporated(double min_pheromone_intensity) const;
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

    std::vector<FoodParticle>::const_iterator begin() const;
    std::vector<FoodParticle>::const_iterator end() const;
  };

  std::vector<CircleWithFood> circles_with_food_vec_;
  std::default_random_engine engine_;

 public:
  inline static std::string const DEFAULT_FILEPATH_{
      "./assets/simulations/map_1/food/food.dat"};
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

  // returns:
  //  - true if there was food in the circle (therefore if has also been
  //  removed)
  //  - false if there wasn't any food in the circle (therefore
  //  nothing has been removed)
  //
  // iterators of class Food::Iterator are invalidated if true
  bool removeOneFoodParticleInCircle(Circle const& circle);

  bool loadFromFile(Obstacles const& obstacles,
                    std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_) const;

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

// for the pheromone
// declared outside because it's needed for the hash function
struct PheromonesSquareCoordinate
{
  int x;
  int y;
  friend bool operator==(PheromonesSquareCoordinate const& lhs,
                         PheromonesSquareCoordinate const& rhs);
};

// closing temporarily for hash function
} // namespace kape

template<>
struct std::hash<kape::PheromonesSquareCoordinate>
{
  std::size_t
  operator()(kape::PheromonesSquareCoordinate const& coordinate) const noexcept
  {
    std::size_t hash_x = std::hash<int>{}(coordinate.x);
    std::size_t hash_y = std::hash<int>{}(coordinate.y);
    return hash_x ^ (hash_y << 1);
  }
};

// reopening
namespace kape {
class Pheromones
{
 public:
  enum class Type
  {
    TO_FOOD,
    TO_ANTHILL
  };

 private:
  PheromonesSquareCoordinate
  positionToPheromonesSquareCoordinate(Vector2d const& position) const;
  // returns the position of the top left corner of the square
  Vector2d pheromonesSquareCoordinateToPosition(
      PheromonesSquareCoordinate const& coord) const;

  // set to public for tests
 public:
  // every PERIOD_BETWEEN_EVAPORATION_UPDATE_ each pheromone particle loses 1
  // intensity levels
  inline static double const PERIOD_BETWEEN_EVAPORATION_UPDATE_{1.};

  // below this threshold the pheromone is considered to have evaporated
  inline static double const MIN_PHEROMONE_INTENSITY_MAP_{.5};
  inline static double const DECREASE_PERCENTAGE_AMOUNT_MAP_{0.01};
  inline static double const MIN_PHEROMONE_INTENSITY_OPTIMIZATION_{0.02};
  inline static double const DECREASE_PERCENTAGE_AMOUNT_OPTIMIZATION_{0.001};

 private:
  // has to be > than an ant's circle of vision diameter
  double const SQUARE_LENGTH_;
  std::unordered_map<PheromonesSquareCoordinate, std::deque<PheromoneParticle>>
      pheromones_squares_;
  const Type type_;
  std::default_random_engine random_engine_;
  double time_since_last_evaporation_;

  double MIN_PHEROMONE_INTENSITY_;
  double DECREASE_PERCENTAGE_AMOUNT_;

  using map_const_it =
      std::unordered_map<PheromonesSquareCoordinate,
                         std::deque<PheromoneParticle>>::const_iterator;
  using square_const_it = std::deque<PheromoneParticle>::const_iterator;

  void fillWithNeighbouringPheromonesSquares(
      std::vector<map_const_it>& neighbouring_squares,
      PheromonesSquareCoordinate const& center_square_coordinate) const;

 public:
  class Iterator
  {
   private:
    square_const_it pheromone_particle_it_;
    map_const_it pheromones_square_it_;
    map_const_it pheromones_square_end_it_;

   public:
    explicit Iterator(square_const_it const& pheromone_particle_it,
                      map_const_it const& pheromones_square_it_,
                      map_const_it const& pheromones_square_end_it);
    Iterator& operator++(); // prefix ++
    PheromoneParticle const& operator*() const;
    PheromoneParticle const* operator->() const;
    friend bool operator==(Iterator const& lhs, Iterator const& rhs);
    friend bool operator!=(Iterator const& lhs, Iterator const& rhs);
  };

  // Pheromone members------------------------
  // may throw if ant_circle_of_vision_diameter<=0.
  explicit Pheromones(Type type, double ant_circle_of_vision_diameter,
                      unsigned int seed = 31415u);
  double getPheromonesIntensityInCircle(Circle const& circle) const;
  // returns end() if there were no pheromones in the circle
  Iterator getRandomMaxPheromoneParticleInCircle(Circle const& circle);
  Pheromones::Type getPheromonesType() const;
  std::size_t getNumberOfPheromones() const;
  double getMinPheromoneIntensity() const;
  double getMaxPheromoneIntensity() const;
  // may throw std::invalid_argument if intensity is <= 0.
  void addPheromoneParticle(Vector2d const& position, double intensity);
  void addPheromoneParticle(PheromoneParticle const& particle);
  bool timeToEvaporate(double delta_t);
  // may throw std::invalid_argument if delta_t<0.
  void updateParticlesEvaporation(double delta_t = 0.01);

  void optimizePath(bool optimize_path);

  // renders the pheromones into the supplied std::vector<sf::Vertex>
  // pheromone_to_vertex_pos must be callable as a void function that takes a
  // PheromoneParticle const& and a sf::Vector2f & and puts the
  // PheromoneParticle position on the screen into the Vector2f
  template<class PhToVertexPos>
  void friend renderInto(std::vector<sf::Vertex>& vertices,
                         Pheromones const& pheromones,
                         PhToVertexPos pheromone_to_vertex_pos,
                         sf::Color const& pheromone_color)
  {
    sf::Color color = pheromone_color;
    double const max_pheromone_intensity{pheromones.getMaxPheromoneIntensity()};

    sf::Vector2f position;
    for (auto const& square : pheromones.pheromones_squares_) {
      for (auto const& pheromone_particle : square.second) {
        color.a = static_cast<sf::Uint8>((pheromone_particle.getIntensity()
                                          / max_pheromone_intensity * 255.));
        pheromone_to_vertex_pos(pheromone_particle, position);
        vertices.emplace_back(position, color);
      }
    }
  }

  Iterator begin() const;
  Iterator end() const;
};

class Anthill
{
 private:
  Circle circle_;
  int food_counter_;

 public:
  inline static std::string const DEFAULT_FILEPATH_{
      "./assets/simulations/map_1/anthill/anthill.dat"};
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
  bool loadFromFile(Obstacles const& obstacles,
                    std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_) const;
};
} // namespace kape

#endif