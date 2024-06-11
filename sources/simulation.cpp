#include "simulation.hpp"
#include "ants.hpp"
#include "drawing.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include <fstream>
#include <string>
// TODO:
//  - actually let the user choose a simulation

namespace kape {

bool Simulation::loadSimulation(std::string const& simulation_name,
                                std::string const& simulations_folder_path)
{
  std::string simulation_path{simulations_folder_path + '/' + simulation_name
                              + '/'};

  return obstacles_.loadFromFile(simulation_path + "obstacles/obstacles.dat")
      && anthill_.loadFromFile(obstacles_,
                               simulation_path + "anthill/anthill.dat")
      && food_.loadFromFile(obstacles_, simulation_path + "food/food.dat")
      && ants_.loadFromFile(anthill_, simulation_path + "ants/ants.dat")
      && window_.loadAntAnimationFrames(
          simulation_path + "ants/",
          kape::Ant::ANIMATION_TOTAL_NUMBER_OF_FRAMES);
}
Simulation::Simulation()
    : obstacles_{}
    , anthill_{}
    , food_{}
    , ants_{}
    , to_anthill_ph_{Pheromones::Type::TO_ANTHILL,
                     2. * Ant::CIRCLE_OF_VISION_RADIUS}
    , to_food_ph_{Pheromones::Type::TO_FOOD, 2. * Ant::CIRCLE_OF_VISION_RADIUS}
    , delta_t_{DEFAULT_DELTA_T_}
    , ready_to_run_{false}
    , window_{}
{}

bool Simulation::chooseAndLoadSimulation()
{
  std::string simulation_folder_path;
  std::string chosen_simulation;

  // ...
  // something to actually choose a simulation
  // ...
  simulation_folder_path = DEFAULT_SIMULATIONS_FOLDER_PATH_;
  chosen_simulation      = DEFAULT_SIMULATION_NAME_;
  // ...
  // ...
  // ...

  if (loadSimulation(chosen_simulation, simulation_folder_path)) {
    ready_to_run_ = true;
  } else {
    kape::log << "[ERROR]:\tfrom Simulation::loadSimulation(std::string const& "
                 "simulation_name, std::string const& simulations_folder_path):"
                 "\n\t\t\tTried to load the simulation \""
              << chosen_simulation << "\" from \"" << simulation_folder_path
              << " but failed to do so.\n";

    ready_to_run_ = false;
  }

  return ready_to_run_;
}

bool Simulation::isReadyToRun()
{
  return ready_to_run_;
}

void Simulation::run()
{
  if (!ready_to_run_) {
    return;
  }

  while (window_.isOpen()) {
    ants_.update(food_, to_anthill_ph_, to_food_ph_, anthill_, obstacles_,
                 delta_t_);
    to_anthill_ph_.updateParticlesEvaporation(delta_t_);
    to_food_ph_.updateParticlesEvaporation(delta_t_);

    window_.clear(sf::Color(184, 139, 74));
    window_.draw(ants_);
    window_.draw(food_, to_anthill_ph_, to_food_ph_);
    // for (auto const& ant : ants) {
    //   std::array<kape::Circle, 3> circles_of_vision;
    //   ant.calculateCirclesOfVision(circles_of_vision);
    //   window.draw(circles_of_vision[0], sf::Color::Blue);
    //   window.draw(circles_of_vision[1], sf::Color::Blue);
    //   window.draw(circles_of_vision[2], sf::Color::Blue);
    // }
    window_.draw(anthill_);
    window_.draw(obstacles_, sf::Color::Yellow);

    window_.display();
    window_.inputHandling();
  }
}
} // namespace kape
