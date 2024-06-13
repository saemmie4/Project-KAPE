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
// may throw std::invalid_argument if pheromone_reserve <= 0.
// may throw std::invalid_argument if pheromone_reserve <= 0.
Ant::Ant(Vector2d const& position, Vector2d const& direction, int current_frame,
         bool has_food, double pheromone_reserve)
    // if norm(direction) == 0. we would be dividing by 0.
    // before checking if norm(direction)==0
    : desired_direction_{norm2(direction) == 0. ? direction
                                                : (direction / norm(direction))}
    , position_{position}
    , velocity_{ANT_SPEED * desired_direction_}
    , has_food_{has_food}
    , pheromone_reserve_{pheromone_reserve}
    // small hack to have the ants put pheromones down immediatly, near the
    // antill (the first was too far away and they missed the anthill)
    , time_since_last_pheromone_release_{PERIOD_BETWEEN_PHEROMONE_RELEASE_
                                         * 1.5}
    , time_since_last_pheromone_search_{0.}
    , current_frame_{current_frame}
{
  if (norm2(desired_direction_) == 0.) {
    throw std::invalid_argument{"the ant's direction can't be null"};
  }

  if (current_frame_ < 0 || current_frame_ > ANIMATION_TOTAL_NUMBER_OF_FRAMES) {
    throw std::invalid_argument{"the current frame must be in [0, "
                                "ANT::ANIMATION_TOTAL_NUMBER_OF_FRAMES)"};
  }

  if (pheromone_reserve_ <= 0.) {
    throw std::invalid_argument{
        "the ant's phermone reserve can't be negative or null"};
  }

  if (pheromone_reserve_ <= 0.) {
    throw std::invalid_argument{
        "the ant's phermone reserve can't be negative or null"};
  }
}

Vector2d const& Ant::getPosition() const
{
  return position_;
}
Vector2d const& Ant::getVelocity() const
{
  return velocity_;
}
Vector2d const& Ant::getDesiredDirection() const
{
  return desired_direction_;
}

double Ant::getFacingAngle() const
{
  return angle(velocity_);
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

void Ant::applyPheromonesInfluence(std::array<Circle, 3> const& cov,
                                   Pheromones& ph_to_follow)
{
  std::vector<Pheromones::Iterator> valid_pheromones;
  for (auto const& circle_of_vision : cov) {
    auto max_intensity_particle_in_circle{
        ph_to_follow.getRandomMaxPheromoneParticleInCircle(circle_of_vision)};
    if (max_intensity_particle_in_circle != ph_to_follow.end()) {
      valid_pheromones.push_back(max_intensity_particle_in_circle);
    }
  }

  if (valid_pheromones.empty()) {
    return;
  }

  auto max_pheromone{
      std::max_element(valid_pheromones.begin(), valid_pheromones.end(),
                       [](auto const& lhs, auto const& rhs) {
                         return lhs->getIntensity() < rhs->getIntensity();
                       })};

  desired_direction_ = (*max_pheromone)->getPosition() - position_;
  desired_direction_ /=
      norm(desired_direction_); // norm can't be null because the circles of
                                // vision are not on the ant
}
void Ant::applyRandomTurning(std::default_random_engine& random_engine)
{
  //   double wander_stenght = .04;
  // std::uniform_real_distribution angle_dist{0., 2 * PI};
  // Vector2d ex_desired_direction{desired_direction_};
  // desired_direction_ +=
  //     wander_stenght * rotate(Vector2d{0., 1.}, angle_dist(random_engine));
  // double norm_desired_direction{norm(desired_direction_)};
  // if (norm_desired_direction != 0.) {
  //   desired_direction_ /= norm_desired_direction;
  // } else {
  //   desired_direction_ = ex_desired_direction;
  // }

  std::normal_distribution angle_randomizer{0., PI / 24.};

  desired_direction_ =
      rotate(desired_direction_, angle_randomizer(random_engine));
}

bool seesTheAnthill(std::array<Circle, 3> const& cov, Anthill const& anthill)
{
  return std::any_of(
      cov.begin(), cov.end(), [&anthill](Circle const& circle_of_vision) {
        return doShapesIntersect(circle_of_vision, anthill.getCircle());
      });
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

  time_since_last_pheromone_search_ += delta_t;
  bool time_to_search_pheromones{false};
  if (time_since_last_pheromone_search_ > PERIOD_BETWEEN_PHEROMONE_SEARCH_) {
    time_since_last_pheromone_search_ -= PERIOD_BETWEEN_PHEROMONE_SEARCH_;
    time_to_search_pheromones = true;
  }
  // we  consider the ant as a bar long        position_ + CIRCLE_OF_VISION_DISTANCE * rotate(facing_dir, angle)); ANTS::ANT_LENGHT that rotates along its
  // center; this rotation is considered a consequence from two forces, one at
  // the top and on at the bottom of the ant, that cooperatively try to rotate
  // its velocity_ vector to align it to the desired_direction_. To try and not
  // overshoot the desired_direction the ants acts with a force that decreases
  // with the alignment of the velocity_ and desired_direction_ vector
  Vector2d current_direction{velocity_ / norm(velocity_)};
  double force_multiplier{current_direction
                          * desired_direction_}; // note: dot-product
  if (force_multiplier < 0.) { // i.e. the vectors are more than PI/2 radians
                               // apart
    force_multiplier = 0.;
  }
  // decreases from 1 to 0 in [PI/2, 0], is = 1 if they are more than PI/2 apart
  // note: it's analogus for negative angles
  force_multiplier = 1. - force_multiplier;
  //+1 : the rotation will be anticlockwise
  //-1 : the rotation will be clockwise
  double force_sign{
      cross_product(current_direction, desired_direction_) > 0. ? +1. : -1.};
  double force{force_sign * force_multiplier * ANT_FORCE_MAX};
  // cosidered the ant's angular velocity to start from 0 rad/s each frame and
  // to be constantly increasing for delta_t sec
  double delta_theta{6. * force / (ANT_MASS * ANT_LENGTH) * delta_t * delta_t};
  // we aren't just rotating the current_velocity to be sure its norm =
  // ANT_SPEED even if there are small errors in floatingnpoint arithmatic
  current_direction = rotate(current_direction, delta_theta);
  velocity_         = current_direction * ANT_SPEED;
  position_ += delta_t * velocity_;

  //[0]: left [1]: center [2]: right
  std::array<Circle, 3> circles_of_vision;
  calculateCirclesOfVision(circles_of_vision);

  // release pheromones
  if (time_to_release_pheromone) {
    if (pheromone_reserve_ > MIN_PHEROMONE_RESERVE_TO_RELEASE) {
      double pheromone_intensity{pheromone_reserve_
                                 * PERCENTAGE_DECREASE_PHEROMONE_RELEASE};
      if (has_food_) {
        to_food_ph.addPheromoneParticle(position_, pheromone_intensity);
      } else {
        to_anthill_ph.addPheromoneParticle(position_, pheromone_intensity);
      }
      pheromone_reserve_ *= (1. - PERCENTAGE_DECREASE_PHEROMONE_RELEASE);
    }
  }

  // avoid obstacles
  double angle_to_avoid_obstacles{calculateAngleToAvoidObstacles(
      circles_of_vision, obstacles, random_engine)};
  if (angle_to_avoid_obstacles != 0.) {
    velocity_          = rotate(velocity_, angle_to_avoid_obstacles);
    desired_direction_ = velocity_ / norm(velocity_);
    return;
  }

  // search for food in circles_of_vision
  if (!has_food_) {
    for (auto const& cov : circles_of_vision) {
      if (food.removeOneFoodParticleInCircle(cov)) {
        has_food_          = true;
        pheromone_reserve_ = MAX_PHEROMONE_RESERVE;
        velocity_ *= -1.;
        return;
      }
    }
  }

  // deal with anthill
  if (anthill.isInside(position_)) { // inside anthill
    pheromone_reserve_ = MAX_PHEROMONE_RESERVE;

    if (has_food_) {
      anthill.addFood();
      has_food_ = false;
      velocity_ *= -1;
      desired_direction_ = velocity_ / norm(velocity_);
      return;
    }
  } else if (seesTheAnthill(circles_of_vision, anthill)
             && has_food_) { // we see the anthill and we have food
    desired_direction_ = (anthill.getCenter() - position_)
                       / norm(anthill.getCenter() - position_);
    return;
  }

  // follow pheromones
  if (time_to_search_pheromones) {
    Pheromones& pheromones_to_follow{has_food_ ? to_anthill_ph : to_food_ph};
    applyPheromonesInfluence(circles_of_vision, pheromones_to_follow);
    applyRandomTurning(random_engine);
  }
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

std::size_t Ants::getNumberOfAnts() const{
  return ants_vec_.size();
}

void Ants::addAntsAroundCircle(Circle const& circle, std::size_t number_of_ants)
{
  // nothing to do
  if (number_of_ants == 0) {
    return;
  }

  ants_vec_.reserve(number_of_ants);
  int counter{0};
  std::uniform_real_distribution dist(0., 2*PI);
  std::uniform_int_distribution starting_frame_generator{
      0, Ant::ANIMATION_TOTAL_NUMBER_OF_FRAMES - 1};
  std::generate_n(
      std::back_inserter(ants_vec_), number_of_ants,
      [&circle, &counter, &starting_frame_generator, this, &dist]() {
        Vector2d facing_direction{
            rotate(Vector2d{0., 1.}, counter * (dist(random_engine_)))};
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