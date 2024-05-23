#include "ants.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include <algorithm> //for generate_n
#include <array>     //for circles of vision of the ant
#include <cmath>
#include <fstream>   //for ofstream and ifstream
#include <random>    //for random turning
#include <stdexcept> //invalid_argument
namespace kape {

// TODO:
//-improve the ant's update method implementation

void Ant::calculateCirclesOfVision(
    std::array<Circle, 3>& circles_of_vision) const
{
  // note: velocity can't be null for class invariant
  Vector2d facing_dir{velocity_ / norm(velocity_)};

  double angle{CIRCLE_OF_VISION_ANGLE};
  for (auto& cov : circles_of_vision) {
    cov.setCircleRadius(CIRCLE_OF_VISION_RADIUS);
    cov.setCircleCenter(
        position_ + CIRCLE_OF_VISION_DISTANCE * rotate(facing_dir, angle));
    angle -= CIRCLE_OF_VISION_ANGLE;
  }
}

// may throw std::invalid_argument if direction is null
// may throw std::invalid_argument if current_frame isn't in
//     [0, ANIMATION_TOTAL_NUMBER_OF_FRAMES)
Ant::Ant(Vector2d const& position, Vector2d const& direction, int current_frame,
         bool has_food)
    // if norm(direction) == 0. we would be dividing by 0.
    // before checking if norm(direction)==0
    : desired_direction_{norm2(direction) == 0. ? direction
                                                : (direction / norm(direction))}
    , position_{position}
    , velocity_{ANT_SPEED * desired_direction_}
    , has_food_{has_food}
    , time_since_last_pheromone_release_{0.}
    , current_frame_{current_frame}
{
  if (norm2(desired_direction_) == 0.) {
    throw std::invalid_argument{"the ant's direction can't be null"};
  }

  if (current_frame_ < 0 || current_frame_ > ANIMATION_TOTAL_NUMBER_OF_FRAMES) {
    throw std::invalid_argument{"the current frame must be in [0, "
                                "ANT::ANIMATION_TOTAL_NUMBER_OF_FRAMES)"};
  }
}

Vector2d const& Ant::getPosition() const
{
  return position_;
}

double Ant::getFacingAngle() const
{
  if (velocity_.x == 0. && velocity_.y == 0.) {
    return 0.;
  }

  // computes the angle in [-pi, +pi]
  return atan2(velocity_.y, velocity_.x);
}

bool Ant::hasFood() const
{
  return has_food_;
}

double Ant::calculateAngleToAvoidObstacles(
    std::array<Circle, 3> const& cov, Obstacles obs,
    std::default_random_engine& random_engine) const
{
  double const LEFT_RIGHT_ANGLE{PI / 6.};
  double const AHEAD_ANGLE{PI / 2.};
  double const AHED_ANGLE_MULTIPLIER{4.};

  double rotate_by_angle{0.};

  bool any_obs_left{obs.anyObstaclesInCircle(cov[0])};
  bool any_obs_ahead{obs.anyObstaclesInCircle(cov[1])};
  bool any_obs_right{obs.anyObstaclesInCircle(cov[2])};

  if (!any_obs_left && any_obs_ahead && !any_obs_right) {
    std::uniform_int_distribution coinflip{0, 1};
    // pick at random left or right
    return AHEAD_ANGLE * (coinflip(random_engine) == 0 ? -1 : 1);
  }

  if (any_obs_left) {
    rotate_by_angle -= LEFT_RIGHT_ANGLE;
  }
  if (any_obs_right) {
    rotate_by_angle += LEFT_RIGHT_ANGLE;
  }
  if (any_obs_ahead) {
    rotate_by_angle = 2 * AHEAD_ANGLE - AHED_ANGLE_MULTIPLIER * rotate_by_angle;
  }

  return rotate_by_angle;
}

double Ant::calculateAngleFromPheromones(std::array<Circle, 3> const& cov,
                                         Pheromones const& ph_to_follow) const
{
  double const ANGLE_OF_ROTATION{PI / 6.};

  struct angle_result
  {
    double weighted_angle;
    int sum_of_weights;
    double angle_of_cov;
  };

  angle_result starting{0., 0, ANGLE_OF_ROTATION};

  auto [weighted_angle, sum_of_weights, useless]{std::accumulate(
      cov.begin(), cov.end(), starting,
      [&ph_to_follow, ANGLE_OF_ROTATION](angle_result sum,
                                         Circle const& circle_of_vision) {
        int cov_weight{
            ph_to_follow.getPheromonesIntensityInCircle(circle_of_vision)};

        sum.weighted_angle += sum.angle_of_cov * cov_weight;
        sum.sum_of_weights += cov_weight;
        sum.angle_of_cov -= ANGLE_OF_ROTATION;
        return sum;
      })};

  if (sum_of_weights == 0) {
    return 0.;
  }
  return weighted_angle / sum_of_weights;
}

/*double Ant::calculateAngleFromPheromones(
    std::array<Circle, 3> const& cov, Pheromones const& ph_to_follow,
    std::default_random_engine& random_engine) const
{
  double const DEFAULT_ANGLE{PI / 500.};
  std::array<int, 3> weights;

  int sum{0};
  for (unsigned long int i = 0; i < 3; i++) // ORRIBILE
  {
    weights[i] = ph_to_follow.getPheromonesIntensityInCircle(cov[i]) + 10;
    sum += weights[i];
  }

  for (unsigned long int i = 0; i < 3; i++) // ORRIBILE
  {
    weights[i] += sum/3;
  }



  std::discrete_distribution<int> the_chooser{weights.begin(), weights.end()};
  return DEFAULT_ANGLE * (1 - the_chooser(random_engine));
}*/
double
Ant::calculateRandomTurning(std::default_random_engine& random_engine) const
{
  std::normal_distribution angle_randomizer{0., PI / 50.};

  return angle_randomizer(random_engine);

  return 0.;
}

// may throw invalid_argument if to_anthill_ph isn't of type
// Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
// Pheromones::Type::TO_FOOD
void Ant::update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
                 Anthill& anthill, Obstacles const& obstacles,
                 std::default_random_engine& random_engine, double delta_t)
{
  if (to_anthill_ph.getPheromonesType() != Pheromones::Type::TO_ANTHILL) {
    throw std::invalid_argument{
        "The parameter to_anthill_ph, passed to Ant::update(), isn't of type "
        "Pheromones::Type::TO_ANTHILL"};
  }
  if (to_food_ph.getPheromonesType() != Pheromones::Type::TO_FOOD) {
    throw std::invalid_argument{
        "The parameter to_food_ph, passed to Ant::update(), isn't of type "
        "Pheromones::Type::TO_FOOD"};
  }
  if (delta_t < 0.) {
    throw std::invalid_argument{"delta_t can't be negative"};
  }

  time_since_last_pheromone_release_ += delta_t;
  bool time_to_release_pheromone{false};
  if (time_since_last_pheromone_release_ > PERIOD_BETWEEN_PHEROMONE_RELEASE_) {
    time_since_last_pheromone_release_ -= PERIOD_BETWEEN_PHEROMONE_RELEASE_;
    time_to_release_pheromone = true;
  }


  position_ += delta_t * velocity_;

  //[0]: left [1]: center [2]: right
  std::array<Circle, 3> circles_of_vision;
  calculateCirclesOfVision(circles_of_vision);

  if (has_food_) {                     // has food
    if (anthill.isInside(position_)) { // inside anthill
      anthill.addFood();
      has_food_ = false;
      velocity_ *= -1.;
      calculateCirclesOfVision(circles_of_vision);
    } else { // outside the anthill
      if (time_to_release_pheromone) {
        to_food_ph.addPheromoneParticle(position_);
      }
    }
  } else { // has no food
    if (time_to_release_pheromone) {
      to_anthill_ph.addPheromoneParticle(position_);
    }
    for (auto const& cov : circles_of_vision) {
      if (food.removeOneFoodParticleInCircle(cov)) {
        has_food_ = true;
        velocity_ *= -1.;
        calculateCirclesOfVision(circles_of_vision);
        break;
      }
    }
  }

  // avoid obstacles
  double angle_to_avoid_obstacles{calculateAngleToAvoidObstacles(
      circles_of_vision, obstacles, random_engine)};

  if (angle_to_avoid_obstacles != 0.) {
    velocity_ = rotate(velocity_, angle_to_avoid_obstacles);
    calculateCirclesOfVision(circles_of_vision);
  }

  // follow appropriate pheromone + randomness
  Pheromones& pheromone_to_follow{has_food_ ? to_anthill_ph : to_food_ph};
  double angle_chosen{calculateAngleFromPheromones(
      circles_of_vision,
      pheromone_to_follow)}; // add random_engine as a third parameter if using
                             // the second method for calculating the angle
  angle_chosen += calculateRandomTurning(random_engine);

  velocity_ = rotate(velocity_, angle_chosen);
}

int Ant::getCurrentFrame() const
{
  return current_frame_;
}
void Ant::goToNextFrame()
{
  current_frame_ = (current_frame_ + 1) % ANIMATION_TOTAL_NUMBER_OF_FRAMES;
}

// Ants class implementation---------------------
// may throw std::invalid_argument if direction is null
void Ants::addAnt(Vector2d const& position, Vector2d const& direction,
                  int current_frame, bool has_food)
{
  ants_vec_.push_back(Ant{position, direction, current_frame, has_food});
}
void Ants::addAnt(Ant const& ant)
{
  ants_vec_.push_back(ant);
}

Ants::Ants(unsigned int seed)
    : ants_vec_{}
    , random_engine_{seed}
    , time_since_last_frame_change{0.}
{}

void Ants::addAntsAroundCircle(Circle const& circle, std::size_t number_of_ants)
{
  // nothing to do
  if (number_of_ants == 0) {
    return;
  }

  ants_vec_.reserve(number_of_ants);
  double const delta_angle{2. * PI / static_cast<double>(number_of_ants)};
  int counter{0};
  std::uniform_int_distribution starting_frame_generator{
      0, Ant::ANIMATION_TOTAL_NUMBER_OF_FRAMES - 1};
  std::generate_n(
      std::back_inserter(ants_vec_), number_of_ants,
      [&circle, &counter, delta_angle, &starting_frame_generator, this]() {
        Vector2d facing_direction{
            rotate(Vector2d{0., 1.}, counter * delta_angle)};
        ++counter;
        return Ant{circle.getCircleCenter()
                       + circle.getCircleRadius() * facing_direction,
                   facing_direction, starting_frame_generator(random_engine_)};
      });
}

// may throw std::invalid_argument if to_anthill_ph isn't of type
// Pheromones::Type::TO_ANTHILL or if to_food_ph isn't of type
// Pheromones::Type::TO_FOOD
// may throw std::invalid_argument if delta_t < 0.
void Ants::update(Food& food, Pheromones& to_anthill_ph, Pheromones& to_food_ph,
                  Anthill& anthill, Obstacles const& obstacles, double delta_t)
{
  bool change_frame{false};
  time_since_last_frame_change += delta_t;
  if (time_since_last_frame_change >= ANIMATION_TIME_BETWEEN_FRAMES) {
    time_since_last_frame_change -= ANIMATION_TIME_BETWEEN_FRAMES;
    change_frame = true;
  }

  for (auto& ant : ants_vec_) {
    ant.update(food, to_anthill_ph, to_food_ph, anthill, obstacles,
               random_engine_, delta_t);
    if (change_frame) {
      ant.goToNextFrame();
    }
  }
}

bool Ants::loadFromFile(Anthill const& anthill, std::string const& filepath)
{
  std::ifstream file_in{filepath, std::ios::in};

  // failed to open the file
  if (!file_in.is_open()) {
    kape::log << "[ERROR]:\tfrom Ants::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  std::size_t number_of_ants;
  file_in >> number_of_ants;

  addAntsAroundCircle(anthill.getCircle(), number_of_ants);

  std::string end_check;
  file_in >> end_check;
  // reached the eof too early or too late->the read failed
  if (end_check != "END") {
    ants_vec_.clear();
    kape::log << "[ERROR]:\tfrom Ants::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    return false;
  }
  return true;
}

bool Ants::saveToFile(std::string const& filepath)
{
  std::ofstream file_out{filepath, std::ios::out | std::ios::trunc};

  // failed to open the file
  if (!file_out.is_open()) {
    kape::log << "[ERROR]:\tfrom Ants::loadFromFile(std::string const& "
                 "filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  file_out << ants_vec_.size() << '\n';

  file_out << "END\n";

  // no need to call file_out.close() because it's done by its deconstructor
  return true;
}

std::vector<Ant>::const_iterator Ants::begin() const
{
  return ants_vec_.cbegin();
}

std::vector<Ant>::const_iterator Ants::end() const
{
  return ants_vec_.cend();
}

} // namespace kape
