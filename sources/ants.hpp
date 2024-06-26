#ifndef ANTS_HPP
#define ANTS_HPP

#include "environment.hpp" //Pheromones, Food, Obstacles
#include "geometry.hpp"    //Vector2d
#include <array>
#include <random>

namespace kape {

class Ant
{
 private:
  // every PERIOD_BETWEEN_PHEROMONE_RELEASE_ the ant releases a pheromone
  inline static double const PERIOD_BETWEEN_PHEROMONE_RELEASE_{.1};
  inline static double const PERIOD_BETWEEN_PHEROMONE_SEARCH_{.25};

  Vector2d desired_direction_;
  Vector2d position_;
  Vector2d velocity_;
  bool has_food_;
  double pheromone_reserve_;
  double time_since_last_pheromone_release_;
  double time_since_last_pheromone_search_;
  int current_frame_;

 public:
  inline static double const ANT_LENGTH{0.005};   // 0.5 cm
  inline static double const ANT_MASS{5.e-6};     // 5 milligrams
  inline static double const ANT_SPEED{0.025};    // 2.5 cm/s
  inline static double const ANT_FORCE_MAX{3e-6}; // 0.000003 N

  inline static double const CIRCLE_OF_VISION_RADIUS{ANT_LENGTH / 1.3};
  inline static double const CIRCLE_OF_VISION_DISTANCE{1.5 * ANT_LENGTH};
  inline static double const CIRCLE_OF_VISION_ANGLE{PI / 3.};

  inline static double const MAX_PHEROMONE_RESERVE{2000.};
  // the ant's reserve decreases by 2% every time the ant releases a pheromone
  inline static double const PERCENTAGE_DECREASE_PHEROMONE_RELEASE{0.02};

  inline static int const ANIMATION_TOTAL_NUMBER_OF_FRAMES{4};

  void calculateCirclesOfVision(std::array<Circle, 3>& circles_of_vision) const;
  double calculateAngleToAvoidObstacles(
      std::array<Circle, 3> const& cov, Obstacles const& obs,
      std::default_random_engine& random_engine) const;

  void applyPheromonesInfluence(std::array<Circle, 3> const& cov,
                                Pheromones& ph_to_follow);

  void applyRandomTurning(std::default_random_engine& random_engine);

  // may throw std::invalid_argument if direction is null
  // may throw std::invalid_argument if current_frame isn't in
  //     [0, ANIMATION_TOTAL_NUMBER_OF_FRAMES)
  // may throw std::invalid_argument if pheromone_reserve <= 0.
  explicit Ant(Vector2d const& position, Vector2d const& direction,
               int current_frame, bool has_food = false,
               double pheromone_reserve = MAX_PHEROMONE_RESERVE);

  Vector2d const& getPosition() const;
  Vector2d const& getVelocity() const;
  Vector2d const& getDesiredDirection() const;
  // if velocity == {0.,0.} instead of the angle it returns 0.
  double getFacingAngle() const;
  bool hasFood() const;
  bool timeToReleasePheromone(double delta_t);
  bool timeToSearchPheromone(double delta_t);
  void updatePositionAndVelocity(double delta_t);
  // may throw std::invalid_argument if to_anthill_ph isn't of type
  // Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
  // Pheromones::Type::TO_FOOD
  // may throw std::invalid_argument if delta_t < 0.
  void update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
              Anthill& anthill, Obstacles const& obstacles,
              std::default_random_engine& random_engine, double delta_t = 0.01);

  int getCurrentFrame() const;
  void goToNextFrame();
};

class Ants
{
 private:
  std::vector<Ant> ants_vec_;
  std::default_random_engine random_engine_;
  double time_since_last_frame_change_;

  // may throw std::invalid_argument if direction is null
  void addAnt(Vector2d const& position, Vector2d const& direction,
              int current_frame, bool has_food = false);
  void addAnt(Ant const& ant);

 public:
  inline static std::string const DEFAULT_FILEPATH_{
      "./assets/simulations/map_1/ants/ants.dat"};

  // NOTE: it's in "simulation" time, not real time
  inline static double const ANIMATION_TIME_BETWEEN_FRAMES_{0.03};

  explicit Ants(unsigned int seed = 44444444u);
  size_t getNumberOfAnts() const;
  void addAntsAroundCircle(Circle const& circle, std::size_t number_of_ants);

  bool timeToChangeFrames(double delta_t);
  // may throw std::invalid_argument if to_anthill_ph isn't of type
  // Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
  // Pheromones::Type::TO_FOOD
  // may throw std::invalid_argument if delta_t < 0.
  void update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
              Anthill& anthill, Obstacles const& obstacles,
              double delta_t = 0.01);

  bool loadFromFile(Anthill const& anthill,
                    std::string const& filepath = DEFAULT_FILEPATH_);
  bool saveToFile(std::string const& filepath = DEFAULT_FILEPATH_) const;

  std::vector<Ant>::const_iterator begin() const;
  std::vector<Ant>::const_iterator end() const;
};

} // namespace kape

#endif