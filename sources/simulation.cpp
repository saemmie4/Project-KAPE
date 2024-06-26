#include "simulation.hpp"
#include "ants.hpp"
#include "drawing.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace kape {

bool Simulation::loadConfigFromFile(std::string const& filepath)
{
  std::ifstream file_in{filepath, std::ios::in};

  // failed to open the file
  if (!file_in.is_open()) {
    kape::log << "[ERROR]:\tfrom Simulation::loadConfigFromFile(std::string "
                 "const& filepath):\n\t\t\tCouldn't open file at \""
              << filepath << "\"\n";
    return false;
  }

  bool is_debug;
  bool calculate_ants_average_distances;
  double optimal_line_slope{0.};
  double optimal_line_intercept{0.};

  file_in >> is_debug >> calculate_ants_average_distances;
  if (calculate_ants_average_distances) {
    file_in >> optimal_line_slope >> optimal_line_intercept;
  }

  std::string end_check;
  file_in >> end_check;

  // reached the eof too early or too late->the read failed
  if (end_check != "END") {
    kape::log << "[ERROR]:\tfrom Simulation::loadConfigFromFile(std::string "
                 "const& filepath):\n\t\t\tTried to load from \""
              << filepath << "\" but it was badly formatted\n";
    return false;
  }

  // all valid
  is_debug_                         = is_debug;
  calculate_ants_average_distances_ = calculate_ants_average_distances;
  optimal_line_slope_               = optimal_line_slope;
  optimal_line_intercept_           = optimal_line_intercept;
  return true;
}

bool Simulation::loadSimulation(
    std::filesystem::directory_entry const& simulation_folder_path)
{
  // note: if the simulation_folder_path is badly formatted the calls to
  // loadFromFile will fail on their own
  std::string simulation_path{simulation_folder_path.path().string() + '/'};

  bool correctly_loaded{
      obstacles_.loadFromFile(simulation_path + "obstacles/obstacles.dat")
      && anthill_.loadFromFile(obstacles_,
                               simulation_path + "anthill/anthill.dat")
      && food_.loadFromFile(obstacles_, simulation_path + "food/food.dat")
      && ants_.loadFromFile(anthill_, simulation_path + "ants/ants.dat")
      && window_.loadAntAnimationFrames(
          simulation_path + "ants/",
          kape::Ant::ANIMATION_TOTAL_NUMBER_OF_FRAMES)
      && loadConfigFromFile(simulation_path + "config.txt")};

  if (correctly_loaded) {
    to_anthill_ph_.optimizePath(calculate_ants_average_distances_);
    to_food_ph_.optimizePath(calculate_ants_average_distances_);
  }

  return correctly_loaded;
}

bool Simulation::timeToRender()
{
  std::chrono::time_point<clock> now{clock::now()};

  long int const microseconds_between_frames{1'000'000 / FRAMERATE};
  if (std::chrono::duration_cast<std::chrono::microseconds>(
          now - last_frame_update_)
          .count()
      > microseconds_between_frames) {
    last_frame_update_ = now;
    return true;
  }

  return false;
}

bool Simulation::timeToCalculateAverageDistances()
{
  time_since_last_ants_average_distances_check_ += simulation_delta_t_;
  if (time_since_last_ants_average_distances_check_
      > PERIOD_BETWEEN_PATH_OPTIMIZATION_CHECK_) {
    time_since_last_ants_average_distances_check_ -=
        PERIOD_BETWEEN_PATH_OPTIMIZATION_CHECK_;
    return true;
  }
  return false;
}

Simulation::Simulation()
    : obstacles_{}
    , anthill_{}
    , food_{}
    , ants_{}
    , to_anthill_ph_{Pheromones::Type::TO_ANTHILL,
                     2. * Ant::CIRCLE_OF_VISION_RADIUS}
    , to_food_ph_{Pheromones::Type::TO_FOOD, 2. * Ant::CIRCLE_OF_VISION_RADIUS}
    , simulation_delta_t_{SIMULATION_DELTA_T_}
    , last_frame_update_{clock::now()}
    , ready_to_run_{false}
    , window_{}
    , time_since_last_ants_average_distances_check_{}
    , average_ants_distance_from_line_{}
    , is_debug_{}
    , calculate_ants_average_distances_{}
    , optimal_line_slope_{}
    , optimal_line_intercept_{}
{}

// gets only the name of the simulation folder starting from the path
//  e.g.:
//  "./assets/simulations/default/" -> "default"
// must be a directory, else it throws a std::invalid_argument
std::string extractSimulationName(
    std::filesystem::directory_entry const& simulation_directory_path)
{
  if (!simulation_directory_path.is_directory()) {
    throw std::invalid_argument{
        "argument passed to extractSimulationFolderName() is not a directory"};
  }

  std::string simulation_name{simulation_directory_path.path().string()};
  bool is_string_correctly_parsed{false};
  while (!is_string_correctly_parsed) {
    std::size_t last_slash_char{simulation_name.find_last_of('/')};
    if (last_slash_char == simulation_name.npos) { // maybe using '\' (?)
      last_slash_char = simulation_name.find_last_of('\\');
    }

    // we want to get the characters after the last '/' (or '\'), which should
    // be the name of the simulation
    if (last_slash_char != simulation_name.npos
        && last_slash_char != simulation_name.size() - 1) {
      simulation_name =
          simulation_name.substr(last_slash_char + 1); // from there to the end
      assert(!simulation_name.empty());
      assert(simulation_name.find('/') == simulation_name.npos);
      assert(simulation_name.find('\\') == simulation_name.npos);
      is_string_correctly_parsed = true;
    } else if (last_slash_char
               == simulation_name.size()
                      - 1) {      // slash at the end of the string?
      simulation_name.pop_back(); // try removing and repeating the parsing
    } else {
      assert(!simulation_name.empty());
      assert(simulation_name.find('/') == simulation_name.npos);
      assert(simulation_name.find('\\') == simulation_name.npos);
      is_string_correctly_parsed = true;
    }
  }

  return simulation_name;
}

// throws std::runtime_error if the chosen number doesn't correspont to a valid
// option
std::size_t chooseOneOptionFromTerminal(std::vector<std::string> const& options)
{
  std::cout << "Please choose one the following simulations, entering the "
               "corresponding number:\n";

  int index{0};
  for (auto const& simulation_name : options) {
    std::cout << "\t[" << index << "] " << simulation_name << '\n';
    ++index;
  }
  std::cout << "\nInput [0-" << options.size() - 1 << "]: ";

  std::size_t chosen_simulation_index;
  std::cin >> chosen_simulation_index;
  if (chosen_simulation_index > options.size() - 1) {
    log << "[ERROR]: from chooseOneOptionFromTerminal(std::vector<std::string> "
           "const& options): "
           "\n\t\t\tThe user's input was invalid.\n";
    throw std::runtime_error{"The given number ["
                             + std::to_string(chosen_simulation_index)
                             + "] doesn't correspond to a valid simulation."};
  }
  return chosen_simulation_index;
}

bool Simulation::chooseAndLoadSimulation()
{
  std::string simulations_folder_path_string{DEFAULT_SIMULATIONS_FOLDER_PATH_};

  std::filesystem::directory_entry simulations_folder{
      simulations_folder_path_string};
  if (!simulations_folder.is_directory()) {
    log << "[ERROR]: from Simulation::chooseAndLoadSimulation(): "
           "\n\t\t\tThe simulation folder path at \""
        << simulations_folder_path_string
        << "\" isn't a directory/doesn't exist\n";
    ready_to_run_ = false;
    return false;
  }

  // the vectors are separated to then pass available_simulations_names to
  // another function
  std::vector<std::filesystem::directory_entry>
      available_simulations_directories;
  std::vector<std::string> available_simulations_names;

  for (auto const& sub_dir :
       std::filesystem::directory_iterator{simulations_folder}) {
    if (sub_dir.is_directory()) {
      available_simulations_directories.push_back(sub_dir);
      available_simulations_names.push_back(extractSimulationName(sub_dir));
    }
  }

  if (available_simulations_directories.empty()) {
    log << "[ERROR]: from Simulation::chooseAndLoadSimulation(): "
           "\n\t\t\tThe simulations folder path at \""
        << simulations_folder_path_string << "\" contains no simulations\n";
    ready_to_run_ = false;
    return false;
  }

  std::size_t chosen_simulation_index{0};
  if (window_.isOpen()) {
    chosen_simulation_index = window_.chooseOneOption(
        available_simulations_names, DEFAULT_BUTTON_COLOR_,
        CHOSEN_BUTTON_COLOR_, BACKGROUND_COLOR_, DEFAULT_BACKGROUND_PATH_);
  } else {
    try {
      chosen_simulation_index =
          chooseOneOptionFromTerminal(available_simulations_names);
    } catch (
        std::runtime_error const& error) { // if the chosen index doesn't
                                           // correspond to a valid simulation
      std::cout << error.what() << '\n';
      ready_to_run_ = false;
      return false;
    }
  }
  assert(available_simulations_directories.size()
         == available_simulations_names.size());
  assert(chosen_simulation_index < available_simulations_directories.size());

  if (loadSimulation(
          available_simulations_directories.at(chosen_simulation_index))) {
    ready_to_run_ = true;
  } else {
    kape::log << "[ERROR]:\tfrom Simulation::loadSimulation(std::string const& "
                 "simulation_name, std::string const& simulations_folder_path):"
                 "\n\t\t\tTried to load the simulation from \""
              << available_simulations_directories.at(chosen_simulation_index)
                     .path()
                     .string()
              << " but failed to do so.\n";

    ready_to_run_ = false;
  }

  return ready_to_run_;
}

bool Simulation::isReadyToRun() const
{
  return ready_to_run_;
}

void averageDistances(Ants const& ants, double slope, double y_intercept,
                      std::vector<double>& average_distances)
{
  double total_distance =
      std::accumulate(ants.begin(), ants.end(), 0.,
                      [slope, y_intercept](double sum, Ant const& ant) {
                        return sum
                             + (std::abs(slope * ant.getPosition().x
                                         + y_intercept - ant.getPosition().y))
                                   / std::sqrt(slope * slope + 1);
                      });
  double avg_distance{total_distance
                      / static_cast<double>(ants.getNumberOfAnts())};
  average_distances.push_back(avg_distance);
}

void Simulation::run()
{
  if (!ready_to_run_) {
    return;
  }

  while (window_.isOpen()) {
    ants_.update(food_, to_anthill_ph_, to_food_ph_, anthill_, obstacles_,
                 simulation_delta_t_);
    to_anthill_ph_.updateParticlesEvaporation(simulation_delta_t_);
    to_food_ph_.updateParticlesEvaporation(simulation_delta_t_);

    // only if it's a simulation where we know which is the optimal path
    if (calculate_ants_average_distances_) {
      if (timeToCalculateAverageDistances()) {
        averageDistances(ants_, optimal_line_slope_, optimal_line_intercept_,
                         average_ants_distance_from_line_);
      }
    }

    if (timeToRender()) {
      window_.clear(BACKGROUND_COLOR_);
      window_.draw(ants_, is_debug_);
      window_.draw(food_, to_anthill_ph_, to_food_ph_, FOOD_COLOR_,
                   TO_ANTHILL_PHEROMONES_COLOR_, TO_FOOD_PHEROMONES_COLOR_);
      window_.draw(anthill_, ANTHILL_COLOR_);
      window_.draw(obstacles_, OBSTACLES_COLOR_);
      window_.display();
      window_.inputHandling();
    }
  }

  if (calculate_ants_average_distances_) {
    graphPoints(average_ants_distance_from_line_);
  }
}
} // namespace kape
