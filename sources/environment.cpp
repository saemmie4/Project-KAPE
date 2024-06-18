#include "environment.hpp"
#include "drawing.hpp"
#include "geometry.hpp"
#include "logger.hpp"
#include <algorithm> //for find_if and remove_if any_of
#include <cassert>
#include <cmath> //for std::ceil and something else
#include <cstring>
#include <deque>
#include <fstream>
#include <numeric> //for accumulate
#include <random>
#include <stdexcept> //invalid_argument
#include <vector>

// TODO:
//  - find a way to use algorithms in removeOneFoodParticleInCircle()

namespace kape {

// implementation of class Obstacles-----------------------------------

Obstacles::Obstacles()
{}

std::size_t Obstacles::getNumberOfObstacles() const
{
  return obstacles_vec_.size();
}
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

bool Obstacles::loadFromFile(std::string const& filepath)
{
  std::ifstream file_in{filepath, std::ios::in};

  // failed to open the file
  if (!file_in.is_open()) {
    kape::log << "[ERROR]:\tfrom Obstacles::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  std::size_t num_obstacles;
  file_in >> num_obstacles;

  // badly formatted file
  if (file_in.eof()) {
    kape::log << "[ERROR]:\tfrom Obstacles::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    return false;
  }

  obstacles_vec_.reserve(num_obstacles);
  try {
    std::generate_n(
        std::back_inserter(obstacles_vec_), num_obstacles, [&file_in]() {
          double top_left_corner_x;
          double top_left_corner_y;
          double width;
          double height;

          file_in >> top_left_corner_x >> top_left_corner_y >> width >> height;
          return Rectangle{Vector2d{top_left_corner_x, top_left_corner_y},
                           width, height};
        });
  } catch (std::invalid_argument const& error) {
    kape::log << "[ERROR]:\tfrom Obstacles::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tthrown exception std::invalid_argument "
                 "with message: \n\t\t\t"
              << error.what() << '\n';
  }

  std::string end_check;
  file_in >> end_check;
  // reached the eof too early->the read failed
  if (end_check != "END") {
    kape::log << "[ERROR]:\tfrom Obstacles::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    obstacles_vec_.clear();
    return false;
  }

  return true;
}
bool Obstacles::saveToFile(std::string const& filepath)
{
  std::ofstream file_out{filepath, std::ios::out | std::ios::trunc};

  // failed to open the file
  if (!file_out.is_open()) {
    kape::log << "[ERROR]:\tfrom Obstacles::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  file_out << obstacles_vec_.size() << '\n';
  for (auto const& obs : obstacles_vec_) {
    file_out << obs.getRectangleTopLeftCorner().x << '\t'
             << obs.getRectangleTopLeftCorner().y << '\t'
             << obs.getRectangleWidth() << '\t' << obs.getRectangleHeight()
             << '\n';
  }

  file_out << "END\n";

  // no need to call file_out.close() because it's done by its deconstructor
  return true;
}

// FoodParticle class Implementation--------------------------
FoodParticle::FoodParticle(Vector2d const& position)
    : position_{position}
{}

// FoodParticle::FoodParticle(FoodParticle const& food_particle)
//     : FoodParticle{food_particle.getPosition()}
// {}

Vector2d const& FoodParticle::getPosition() const
{
  return position_;
}

// FoodParticle& FoodParticle::operator=(FoodParticle const& rhs)
// {
//   if (&rhs != this) {
//     position_ = rhs.position_;
//   }
//   return *this;
// }

// PheromoneParticle class Implementation--------------------------
// may throw std::invalid_argument if intensity <= 0.
PheromoneParticle::PheromoneParticle(Vector2d const& position, double intensity)
    : position_{position}
    , intensity_{intensity}
{
  if (intensity_ <= 0.)
    throw std::invalid_argument{
        "The pheromone's intensity can't be negative or null "};
}

Vector2d const& PheromoneParticle::getPosition() const
{
  return position_;
}

double PheromoneParticle::getIntensity() const
{
  return intensity_;
}

// may throw std::invalid_argument if decrease_percentage_amount isn't in [0, 1)
void PheromoneParticle::decreaseIntensity(double decrease_percentage_amount,
                                          double min_pheromone_intensity)
{
  if (decrease_percentage_amount < 0. || decrease_percentage_amount >= 1.) {
    throw std::invalid_argument{
        "The decrease_percentage_amount can't be outside [0,1)"};
  }

  intensity_ *= (1. - decrease_percentage_amount);

  // to avoid it going to 0 because of finite double precision
  if (intensity_ < min_pheromone_intensity) {
    intensity_ = min_pheromone_intensity;
  }
}

bool PheromoneParticle::hasEvaporated(double min_pheromone_intensity) const
{
  return intensity_ <= min_pheromone_intensity;
}

// Food Class implementation -----------------------------------

// Food::CircleWithFood class implementation--------------------
// may throw std::invalid_argument if number_of_food_particles < 0 or if the
// circle intersects with any of the obstacles
Food::CircleWithFood::CircleWithFood(Circle const& circle,
                                     std::size_t number_of_food_particles,
                                     Obstacles const& obstacles,
                                     std::default_random_engine& engine)
    : circle_{circle}
    , food_vec_{}
{
  // if the circle intersects any obstacles
  if (std::any_of(obstacles.begin(), obstacles.end(),
                  [&circle](Rectangle const& rectangle) {
                    return doShapesIntersect(circle, rectangle);
                  })) {
    throw std::invalid_argument{"can't construct a CircleWithFood object if "
                                "its circle intersects any obstacle"};
  }

  std::uniform_real_distribution angle_distribution{0., 2 * PI};
  // sigma = radius/3. makes the probability of having a generated distance from
  // the center = 99.7%.
  std::normal_distribution center_distance_distribution{
      0., circle.getCircleRadius() / 3.};

  std::generate_n(
      std::back_inserter(food_vec_), number_of_food_particles,
      [&circle, &center_distance_distribution, &angle_distribution, &engine]() {
        double angle{angle_distribution(engine)};
        double center_distance{std::abs(center_distance_distribution(engine))};
        if (center_distance > circle.getCircleRadius()) {
          center_distance = circle.getCircleRadius();
        }

        Vector2d position{rotate(Vector2d{0., 1.}, angle)};
        position *= center_distance;
        position += circle.getCircleCenter();
        return FoodParticle{position};
      });
}

Circle const& Food::CircleWithFood::getCircle() const
{
  return circle_;
}

std::size_t Food::CircleWithFood::getNumberOfFoodParticles() const
{
  return food_vec_.size();
}

bool Food::CircleWithFood::removeOneFoodParticleInCircle(Circle const& circle)
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

bool Food::CircleWithFood::isThereFoodLeft() const
{
  return !food_vec_.empty();
}

std::vector<FoodParticle>::const_iterator Food::CircleWithFood::begin() const
{
  return food_vec_.cbegin();
}
std::vector<FoodParticle>::const_iterator Food::CircleWithFood::end() const
{
  return food_vec_.cend();
}

// actual Food class implementation-------------------------------------------

Food::Food(unsigned int seed)
    : circles_with_food_vec_{}
    , engine_{seed}
{}

std::size_t Food::getNumberOfFoodParticles() const
{
  return std::accumulate(
      circles_with_food_vec_.begin(), circles_with_food_vec_.end(), 0ul,
      [](std::size_t sum, CircleWithFood const& circle_with_food) {
        return sum + circle_with_food.getNumberOfFoodParticles();
      });
}
// void Food::addFoodParticle(Vector2d const& position)
// {
//   food_vec_.push_back(FoodParticle{position});
// }

// void Food::addFoodParticle(FoodParticle const& food_particle)
// {
//   food_vec_.push_back(food_particle);
// }

// returns:
//  - true if it generated the food_particles (0 if number_of_particles==0 ->
//    the function did nothing)
//  - false if it didn't generate any particle, i.e. the circle intersects at
//    least in part one obstacle
//
// iterators of class Food::Iterator are invalidated
bool Food::generateFoodInCircle(Circle const& circle,
                                std::size_t number_of_food_particles,
                                Obstacles const& obstacles)
{
  // if the circle intersects any obstacles
  if (std::any_of(obstacles.begin(), obstacles.end(),
                  [&circle](Rectangle const& rectangle) {
                    return doShapesIntersect(circle, rectangle);
                  })) {
    return false;
  }

  // nothing to do...
  if (number_of_food_particles == 0) {
    return true;
  }

  // generating a new circle_with_food.
  // no need for try catch because we already checked that number of particles
  // is > 0 and that the circle doesn't intersect any obstacle
  circles_with_food_vec_.push_back(
      CircleWithFood{circle, number_of_food_particles, obstacles, engine_});

  return true;
}

bool Food::isThereFoodLeft() const
{
  return !circles_with_food_vec_.empty();
}

// iterators of class Food::Iterator are invalidated if true
bool Food::removeOneFoodParticleInCircle(Circle const& circle)
{
  auto circles_with_food_it{circles_with_food_vec_.begin()};
  for (; circles_with_food_it != circles_with_food_vec_.end();
       ++circles_with_food_it) {
    if (!doShapesIntersect(circle, circles_with_food_it->getCircle())) {
      continue;
    }

    if (circles_with_food_it->removeOneFoodParticleInCircle(circle)) {
      if (!circles_with_food_it->isThereFoodLeft()) {
        circles_with_food_vec_.erase(circles_with_food_it);
      }
      return true;
    }
  }
  // no particle found in any of the circles
  return false;
}

bool Food::loadFromFile(Obstacles const& obstacles, std::string const& filepath)
{
  std::ifstream file_in{filepath, std::ios::in};

  // failed to open the file
  if (!file_in.is_open()) {
    kape::log << "[ERROR]:\tfrom Food::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  std::size_t num_circles_with_food;
  file_in >> num_circles_with_food;

  // badly formatted file
  if (file_in.eof()) {
    kape::log << "[ERROR]:\tfrom Food::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    return false;
  }

  circles_with_food_vec_.reserve(num_circles_with_food);
  try {
    std::generate_n(
        std::back_inserter(circles_with_food_vec_), num_circles_with_food,
        [&file_in, &obstacles, this]() {
          double circle_center_x;
          double circle_center_y;
          double circle_radius;
          std::size_t number_of_particles;

          file_in >> circle_center_x >> circle_center_y >> circle_radius
              >> number_of_particles;
          return CircleWithFood{
              Circle{Vector2d{circle_center_x, circle_center_y}, circle_radius},
              number_of_particles, obstacles, engine_};
        });
  } catch (std::invalid_argument const& error) {
    kape::log << "[ERROR]:\tfrom Food::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tthrown exception std::invalid_argument "
                 "with message: \n\t\t\t"
              << error.what() << '\n';
  }

  std::string end_check;
  file_in >> end_check;
  // reached the eof too early->the read failed
  if (end_check != "END") {
    kape::log << "[ERROR]:\tfrom Food::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    circles_with_food_vec_.clear();
    return false;
  }

  return true;
}

bool Food::saveToFile(std::string const& filepath)
{
  std::ofstream file_out{filepath, std::ios::out | std::ios::trunc};

  // failed to open the file
  if (!file_out.is_open()) {
    kape::log << "[ERROR]:\tfrom Food::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  file_out << circles_with_food_vec_.size() << '\n';
  for (auto const& circle_with_food : circles_with_food_vec_) {
    file_out << circle_with_food.getCircle().getCircleCenter().x << '\t'
             << circle_with_food.getCircle().getCircleCenter().y << '\t'
             << circle_with_food.getCircle().getCircleRadius() << '\t'
             << circle_with_food.getNumberOfFoodParticles() << '\n';
  }

  file_out << "END\n";

  // no need to call file_out.close() because it's done by its deconstructor
  return true;
}

// class Food::iterator implementation-------------------------------------
Food::Iterator::Iterator(
    std::vector<FoodParticle>::const_iterator const& food_particle_it,
    std::vector<CircleWithFood>::const_iterator const& circle_with_food_it,
    std::vector<CircleWithFood>::const_iterator const& circle_with_food_back_it)
    : food_particle_it_{food_particle_it}
    , circle_with_food_it_{circle_with_food_it}
    , circle_with_food_back_it_{circle_with_food_back_it}
{}

Food::Iterator& Food::Iterator::operator++() // prefix ++
{
  ++food_particle_it_;

  // if we're at the end of the current circle with food
  if (food_particle_it_ == circle_with_food_it_->end()) {
    // if we're NOT the end of the last circle with food
    if (circle_with_food_it_ != circle_with_food_back_it_) {
      food_particle_it_ = (++circle_with_food_it_)->begin();
    }
  }
  return *this;
}

FoodParticle const& Food::Iterator::operator*() const
{
  return *food_particle_it_;
}

bool operator==(Food::Iterator const& lhs, Food::Iterator const& rhs)
{
  return lhs.food_particle_it_ == rhs.food_particle_it_;
}

bool operator!=(Food::Iterator const& lhs, Food::Iterator const& rhs)
{
  return lhs.food_particle_it_ != rhs.food_particle_it_;
}

Food::Iterator Food::begin() const
{
  if (circles_with_food_vec_.empty()) {
    return Food::Iterator{std::vector<FoodParticle>::const_iterator{},
                          circles_with_food_vec_.end(),
                          circles_with_food_vec_.end()};
  }

  return Food::Iterator{circles_with_food_vec_.begin()->begin(),
                        circles_with_food_vec_.begin(),
                        circles_with_food_vec_.end() - 1};
}

Food::Iterator Food::end() const
{
  if (circles_with_food_vec_.empty()) {
    return Food::Iterator{std::vector<FoodParticle>::iterator{},
                          circles_with_food_vec_.end(),
                          circles_with_food_vec_.end()};
  }

  return Food::Iterator{circles_with_food_vec_.back().end(),
                        circles_with_food_vec_.end() - 1,
                        circles_with_food_vec_.end() - 1};
}

// Pheromones class implementation ------------------------------

PheromonesSquareCoordinate
Pheromones::positionToPheromonesSquareCoordinate(Vector2d const& position) const
{
  PheromonesSquareCoordinate coord;
  coord.x = static_cast<int16_t>(std::ceil(position.x / SQUARE_LENGTH_)) - 1;
  coord.y = static_cast<int16_t>(std::ceil(position.y / SQUARE_LENGTH_));
  return coord;
}

// returns the position of the top left corner of the square
Vector2d Pheromones::pheromonesSquareCoordinateToPosition(
    PheromonesSquareCoordinate const& coord) const
{
  Vector2d top_left_corner{static_cast<double>(coord.x),
                           static_cast<double>(coord.y)};
  return SQUARE_LENGTH_ * top_left_corner;
}

void Pheromones::fillWithNeighbouringPheromonesSquares(
    std::vector<map_const_it>& neighbouring_squares,
    PheromonesSquareCoordinate const& center_square_coordinate) const
{
  int center_x = center_square_coordinate.x;
  int center_y = center_square_coordinate.y;

  for (int delta_x = -1; delta_x <= 1; ++delta_x) {
    for (int delta_y = -1; delta_y <= 1; ++delta_y) {
      map_const_it square_it{pheromones_squares_.find(
          PheromonesSquareCoordinate{center_x + delta_x, center_y + delta_y})};

      if (square_it != pheromones_squares_.end()) {
        assert(!square_it->second.empty());
        neighbouring_squares.push_back(square_it);
      }
    }
  }
}

double Pheromones::getMinimumPheromoneIntensity()
{
  return MIN_PHEROMONE_INTENSITY_;
}

Pheromones::Pheromones(Type type, double ant_circle_of_vision_diameter,
                       unsigned int seed)
    : SQUARE_LENGTH_{2. * ant_circle_of_vision_diameter}
    , pheromones_squares_{}
    , type_{type}
    , random_engine_{seed}
    , time_since_last_evaporation_{0.}
    , MIN_PHEROMONE_INTENSITY_{MIN_PHEROMONE_INTENSITY_MAP_}
    , DECREASE_PERCENTAGE_AMOUNT_{DECREASE_PERCENTAGE_AMOUNT_MAP_}

{
  if (SQUARE_LENGTH_ <= 0.) {
    throw std::invalid_argument{"the ant's circle of vision diameter, passed "
                                "to Pheromones, can't be negative or null"};
  }
}

double Pheromones::getPheromonesIntensityInCircle(Circle const& circle) const
{
  PheromonesSquareCoordinate center_square_coordinate{
      positionToPheromonesSquareCoordinate(circle.getCircleCenter())};

  // get all existing neighbouring squares (the center is also inside
  // neighbouring squares)
  std::vector<map_const_it> neighbouring_squares;
  fillWithNeighbouringPheromonesSquares(neighbouring_squares,
                                        center_square_coordinate);

  if (neighbouring_squares.empty()) {
    return 0.;
  }

  // remove all squares that don't intersect the circle
  neighbouring_squares.erase(
      std::remove_if(neighbouring_squares.begin(), neighbouring_squares.end(),
                     [&circle, this](map_const_it square) {
                       Rectangle square_rect{
                           pheromonesSquareCoordinateToPosition(square->first),
                           SQUARE_LENGTH_, SQUARE_LENGTH_};
                       return !doShapesIntersect(circle, square_rect);
                     }),
      neighbouring_squares.end());

  if (neighbouring_squares.empty()) {
    return 0.;
  }

  // sum of the sums of the particles inside the squares
  return std::accumulate(
      neighbouring_squares.begin(), neighbouring_squares.end(), 0.,
      [&circle](double total_sum, map_const_it square_it) {
        return total_sum
             + std::accumulate(
                   square_it->second.begin(), square_it->second.end(), 0.,
                   [&circle](double square_sum,
                             PheromoneParticle const& pheromone) {
                     return square_sum
                          + (circle.isInside(pheromone.getPosition())
                                 ? pheromone.getIntensity()
                                 : 0.);
                   });
      });
}

// returns end() if there were no pheromones in the circle
Pheromones::Iterator
Pheromones::getRandomMaxPheromoneParticleInCircle(Circle const& circle)
{
  PheromonesSquareCoordinate center_square_coordinate{
      positionToPheromonesSquareCoordinate(circle.getCircleCenter())};

  // get all existing neighbouring squares (the center is also inside
  // neighbouring squares)
  std::vector<map_const_it> neighbouring_squares;
  fillWithNeighbouringPheromonesSquares(neighbouring_squares,
                                        center_square_coordinate);

  if (neighbouring_squares.empty()) {
    return end();
  }

  // remove all squares that don't intersect the circle
  neighbouring_squares.erase(
      std::remove_if(neighbouring_squares.begin(), neighbouring_squares.end(),
                     [&circle, this](map_const_it square) {
                       Rectangle square_rect{
                           pheromonesSquareCoordinateToPosition(square->first),
                           SQUARE_LENGTH_, SQUARE_LENGTH_};
                       return !doShapesIntersect(circle, square_rect);
                     }),
      neighbouring_squares.end());

  if (neighbouring_squares.empty()) {
    return end();
  }

  // for each pheromone we have a 0.1% chance of returning the max of previous
  // intensities (up to that point)
  double probability_of_returning_early{0.001};
  std::uniform_real_distribution<double> distr(0., 1.);

  // i am extreamly sorry for this monstruosity
  // basically we search the pheromone with the highest intensity, but, each
  // time we find a pheromone inside the circle, there's a
  // probability_of_returning_early and returning the max found so far
  Pheromones::Iterator max_intensity_particle{end()};
  for (auto pheromone_square_it{neighbouring_squares.begin()};
       pheromone_square_it != neighbouring_squares.end();
       ++pheromone_square_it) {
    // neighbouring squares should never be empty for class invariant
    assert(!(*pheromone_square_it)->second.empty());
    for (auto pheromone_particle_it{(*pheromone_square_it)->second.begin()};
         pheromone_particle_it != (*pheromone_square_it)->second.end();
         ++pheromone_particle_it) {
      if (circle.isInside(pheromone_particle_it->getPosition())) {
        if (max_intensity_particle == end()) {
          max_intensity_particle =
              Iterator{pheromone_particle_it, *pheromone_square_it,
                       pheromones_squares_.end()};
        } else if (pheromone_particle_it->getIntensity()
                   > (*max_intensity_particle).getIntensity()) {
          max_intensity_particle =
              Iterator{pheromone_particle_it, *pheromone_square_it,
                       pheromones_squares_.end()};
        }

        if (distr(random_engine_) < probability_of_returning_early) {
          return max_intensity_particle;
        }
      }
    }
  }

  return max_intensity_particle;
}

Pheromones::Type Pheromones::getPheromonesType() const
{
  return type_;
}
std::size_t Pheromones::getNumberOfPheromones() const
{
  return std::accumulate(pheromones_squares_.begin(), pheromones_squares_.end(),
                         0UL,
                         [](std::size_t sum, auto const& pheromones_square) {
                           return sum + pheromones_square.second.size();
                         });
}

double Pheromones::getMaxPheromoneIntensity() const
{
  return Ant::MAX_PHEROMONE_RESERVE
       * Ant::PERCENTAGE_DECREASE_PHEROMONE_RELEASE;
}

void Pheromones::addPheromoneParticle(Vector2d const& position,
                                      double intensity)
{
  PheromonesSquareCoordinate square_coord{
      positionToPheromonesSquareCoordinate(position)};
  // pheromones_squares_[square_coord].reserve(400);
  pheromones_squares_[square_coord].emplace_back(position, intensity);
}

void Pheromones::addPheromoneParticle(PheromoneParticle const& particle)
{
  PheromonesSquareCoordinate square_coord{
      positionToPheromonesSquareCoordinate(particle.getPosition())};
  pheromones_squares_[square_coord].push_back(particle);
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

  for (auto& pheromone_square : pheromones_squares_) {
    for (auto& pheromone_particle : pheromone_square.second) {
      pheromone_particle.decreaseIntensity(DECREASE_PERCENTAGE_AMOUNT_, MIN_PHEROMONE_INTENSITY_);
    }
  }

  // remove phermones that have evaporated from the pheromones squares
  for (auto& pheromone_square : pheromones_squares_) {
    pheromone_square.second.erase(
        std::remove_if(pheromone_square.second.begin(),
                       pheromone_square.second.end(),
                       [](PheromoneParticle const& particle) {
                         return particle.hasEvaporated(MIN_PHEROMONE_INTENSITY_MAP_);
                       }),
        pheromone_square.second.end());
    // pheromone_square.second.remove_if([](PheromoneParticle const& particle)
    // {
    //   return particle.hasEvaporated();
    // });
  }

  // remove empty pheromones squares (it's a remove_if)
  for (auto pheromone_square{pheromones_squares_.begin()};
       pheromone_square != pheromones_squares_.end();) {
    if (pheromone_square->second.empty()) {
      pheromone_square = pheromones_squares_.erase(pheromone_square);
    } else {
      ++pheromone_square;
    }
  }
}

void Pheromones::optimizePath(bool optimize_path)
{
  MIN_PHEROMONE_INTENSITY_ = optimize_path ? MIN_PHEROMONE_INTENSITY_OPTIMIZATION_ : MIN_PHEROMONE_INTENSITY_MAP_;
  DECREASE_PERCENTAGE_AMOUNT_ = optimize_path ? DECREASE_PERCENTAGE_AMOUNT_OPTIMIZATION_ : DECREASE_PERCENTAGE_AMOUNT_MAP_;
}


Pheromones::Iterator::Iterator(square_const_it const& pheromone_particle_it,
                               map_const_it const& pheromones_square_it,
                               map_const_it const& pheromones_square_end_it)
    : pheromone_particle_it_{pheromone_particle_it}
    , pheromones_square_it_{pheromones_square_it}
    , pheromones_square_end_it_{pheromones_square_end_it}
{}

Pheromones::Iterator& Pheromones::Iterator::operator++() // prefix ++
{
  ++pheromone_particle_it_;

  // check if we are not at the end of the current square
  if (pheromone_particle_it_ != pheromones_square_it_->second.end()) {
    return *this;
  }

  // we're at the end of the current square
  ++pheromones_square_it_;
  if (pheromones_square_it_
      == pheromones_square_end_it_) { // i.e. we're at the end() of all
                                      // pheromones
    return *this;
  }

  // we're at the end of the current square BUT it wasn't the last square
  pheromone_particle_it_ = pheromones_square_it_->second.begin();
  return *this;
}

PheromoneParticle const& Pheromones::Iterator::operator*() const
{
  return *pheromone_particle_it_;
}

PheromoneParticle const* Pheromones::Iterator::operator->() const
{
  return &(*pheromone_particle_it_);
}

bool operator==(Pheromones::Iterator const& lhs,
                Pheromones::Iterator const& rhs)
{
  // are they pointing to the same thing OR are they both end()?
  return lhs.pheromone_particle_it_ == rhs.pheromone_particle_it_
      || (lhs.pheromones_square_it_ == lhs.pheromones_square_end_it_
          && rhs.pheromones_square_it_ == rhs.pheromones_square_end_it_);
}
bool operator!=(Pheromones::Iterator const& lhs,
                Pheromones::Iterator const& rhs)
{
  return !(lhs == rhs);
}

Pheromones::Iterator Pheromones::begin() const
{
  if (pheromones_squares_.empty()) {
    return end();
  }

  return Pheromones::Iterator{pheromones_squares_.begin()->second.begin(),
                              pheromones_squares_.begin(),
                              pheromones_squares_.end()};
}
Pheromones::Iterator Pheromones::end() const
{
  return Pheromones::Iterator{square_const_it{}, pheromones_squares_.end(),
                              pheromones_squares_.end()};
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

Circle const& Anthill::getCircle() const
{
  return circle_;
}

Vector2d const& Anthill::getCenter() const
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
bool Anthill::isInside(Vector2d const& position) const
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

bool Anthill::loadFromFile(Obstacles const& obstacles,
                           std::string const& filepath)
{
  std::ifstream file_in{filepath, std::ios::in};

  // failed to open the file
  if (!file_in.is_open()) {
    kape::log << "[ERROR]:\tfrom Anthill::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  double circle_center_x;
  double circle_center_y;
  double circle_radius;
  int food_counter;

  file_in >> circle_center_x >> circle_center_y >> circle_radius
      >> food_counter;

  Circle circle_in;
  circle_in.setCircleCenter(Vector2d{circle_center_x, circle_center_y});
  circle_in.setCircleRadius(circle_radius);

  std::string end_check;
  file_in >> end_check;

  // reached the eof too early or too late->the read failed
  if (end_check != "END") {
    kape::log << "[ERROR]:\tfrom Anthill::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    return false;
  }

  if (food_counter < 0) {
    kape::log << "[ERROR]:\tfrom Anthill::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath
              << "\" but the food counter read was a negative number [food "
                 "counter = "
              << food_counter << "] \n";
    return false;
  }

  // check there are no intersections with the obstacles
  if (std::any_of(obstacles.begin(), obstacles.end(),
                  [&circle_in](kape::Rectangle const& obstacle) {
                    return doShapesIntersect(circle_in, obstacle);
                  })) {
    kape::log
        << "[ERROR]:\tfrom Anthill::loadFromFile(std::string const& "
           "filepath):\n\t\t\tTried to load from \""
        << filepath
        << "\" but the anthill would intersect with at least one obstacle \n";
    return false;
  }

  // all valid
  circle_       = circle_in;
  food_counter_ = food_counter;
  return true;
}

bool Anthill::saveToFile(std::string const& filepath)
{
  std::ofstream file_out{filepath, std::ios::out | std::ios::trunc};

  // failed to open the file
  if (!file_out.is_open()) {
    kape::log << "[ERROR]:\tfrom Anthill::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  file_out << circle_.getCircleCenter().x << '\t' << circle_.getCircleCenter().y
           << '\t' << circle_.getCircleRadius() << '\t' << food_counter_
           << '\n';

  file_out << "END\n";

  // no need to call file_out.close() because it's done by its deconstructor
  return true;
}

} // namespace kape