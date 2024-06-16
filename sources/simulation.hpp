#ifndef SIMULATION_HPP
#define SIMULATION_HPP

// TODO:
//  -constructor
//  -deconstructor
//  -run()

#include "ants.hpp"
#include "drawing.hpp"
#include "environment.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <filesystem>
#include <string>
namespace kape {
class Simulation
{
 private:
  // name of the folder containing the simulations
  inline static std::string const DEFAULT_SIMULATIONS_FOLDER_PATH_{
      "./assets/simulations"};
  // name of the simulation to be loaded by default
  inline static std::string const DEFAULT_SIMULATION_NAME_{"default"};
  // as a default the time between simulation updates is 0.01s
  inline static double const SIMULATION_DELTA_T_{0.01};
  inline static long int const FRAMERATE{60}; // frames per second for drawing
  inline static double const PERIOD_BETWEEN_PATH_OPTIMIZATION_CHECK_{
      SIMULATION_DELTA_T_ * 100};
  inline static sf::Color const ANTHILL_COLOR_{166, 75, 42};
  inline static sf::Color const BACKGROUND_COLOR_{185, 148, 112};
  inline static sf::Color const OBSTACLES_COLOR_{111, 78, 55};
  inline static sf::Color const FOOD_COLOR_{95, 111, 82};
  inline static sf::Color const TO_FOOD_PHEROMONES_COLOR_{170, 86, 86};
  inline static sf::Color const TO_ANTHILL_PHEROMONES_COLOR_{86, 113, 137};
  using clock = std::chrono::steady_clock;

  Obstacles obstacles_;
  Anthill anthill_;
  Food food_;
  Ants ants_;
  Pheromones to_anthill_ph_;
  Pheromones to_food_ph_;
  double const simulation_delta_t_;
  std::chrono::time_point<clock> last_frame_update_;

  bool ready_to_run_;
  Window window_;
  double time_since_last_ants_average_distances_check_;
  std::vector<double> average_ants_distance_from_line_;
  bool is_debug_;
  bool calculate_ants_average_distances_;
  double optimal_line_slope_;
  double optimal_line_intercept_;

  bool loadConfigFromFile(std::string const& filepath);
  // returns true if all the objects have been loaded properly, false if at
  // least one failed
  bool loadSimulation(
      std::filesystem::directory_entry const& simulation_folder_path);

  bool timeToRender();
  bool timeToCalculateAverageDistances();

 public:
  explicit Simulation();
  // returns:
  //    - true if it correctly loaded the simulation and is ready to run
  //    - false if it failed to load the simulation and is therefore unable to
  //      run
  bool chooseAndLoadSimulation();
  bool isReadyToRun();
  void run();
};
} // namespace kape

#endif