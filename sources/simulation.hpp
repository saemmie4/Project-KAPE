#ifndef SIMULATION_HPP
#define SIMULATION_HPP

// TODO:
//  -constructor
//  -deconstructor
//  -run()

#include "ants.hpp"
#include "drawing.hpp"
#include "environment.hpp"
#include <string>
#include <filesystem>
#include <chrono>
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
  inline static long int const FRAMERATE{45}; //frames per second for drawing
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
  double time_since_last_ants_position_check_;
  std::vector<double> average_ants_distance_from_line_;

  // returns true if all the objects have been loaded properly, false if at
  // least one failed
  bool loadSimulation(
      std::filesystem::directory_entry const& simulation_folder_path);

  bool timeToRender();
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