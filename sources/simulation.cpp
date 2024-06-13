#include "simulation.hpp"
#include "ants.hpp"
#include "drawing.hpp"
#include "environment.hpp"
#include "logger.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
// TODO:
//  - actually let the user choose a simulation

namespace kape {

bool Simulation::loadSimulation(
    std::filesystem::directory_entry const& simulation_folder_path)
{
  // note: if the simulation_folder_path is badly formatted the calls to
  // loadFromFile will fail on their own
  std::string simulation_path{simulation_folder_path.path().c_str() + '/'};

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
    , time_since_last_ants_position_check_{}
    , average_ants_distance_from_line_{}
{}

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

  std::vector<std::filesystem::directory_entry>
      available_simulations_directories;
  std::vector<std::string> available_simulations_names;
  for (auto const& sub_dir :
       std::filesystem::directory_iterator{simulations_folder}) {
    if (sub_dir.is_directory()) {
      available_simulations_directories.push_back(sub_dir);

      available_simulations_names.push_back(sub_dir.path().string());
      bool is_string_correctly_parsed{false};
      while (!is_string_correctly_parsed) {
        auto& str{available_simulations_names.back()};
        std::size_t last_slash_char{str.find_last_of('/')};
        if (last_slash_char == str.npos) { // maybe using '\' (?)
          last_slash_char = str.find_last_of('\\');
        }

        // we want to get the characters after the last '/', which should be the
        // name of the simulation
        if (last_slash_char != str.npos && last_slash_char != str.size() - 1) {
          str = str.substr(last_slash_char + 1); // from there to the end
          assert(!str.empty());
          assert(str.find('/') == str.npos);
          assert(str.find('\\') == str.npos);
          is_string_correctly_parsed = true;
        } else if (last_slash_char
                   == str.size() - 1) { // slash at the end of the string?
          str.pop_back(); // try removing and repeating the parsing
        } else {
          assert(!str.empty());
          assert(str.find('/') == str.npos);
          assert(str.find('\\') == str.npos);
          is_string_correctly_parsed = true;
        }
      }
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
    chosen_simulation_index =
        window_.chooseOneOption(available_simulations_names);
  }
  assert(available_simulations_directories.size()
         == available_simulations_names.size());
  assert(chosen_simulation_index < available_simulations_directories.size());

  // ...
  // something to actually choose a simulation
  // ...
  // ...
  // ...
  // // ...

  // if (loadSimulation(chosen_simulation, simulation_folder_path)) {
  //   ready_to_run_ = true;
  // } else {
  //   kape::log << "[ERROR]:\tfrom Simulation::loadSimulation(std::string
  //   const& "
  //                "simulation_name, std::string const&
  //                simulations_folder_path):"
  //                "\n\t\t\tTried to load the simulation \""
  //             << chosen_simulation << "\" from \"" <<
  //             simulation_folder_path
  //             << " but failed to do so.\n";

  //   ready_to_run_ = false;
  // }

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

    time_since_last_ants_position_check_ += DEFAULT_DELTA_T_;
    bool time_to_check_ants_position{false};
    if (time_since_last_ants_position_check_ > 6000 * DEFAULT_DELTA_T_) {
      time_since_last_ants_position_check_ -= 6000 * DEFAULT_DELTA_T_;
      time_to_check_ants_position = true;
    }
    if (time_to_check_ants_position) {
      AverageDistances(ants_, 0., 0., average_ants_distance_from_line_);
    }
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

void AverageDistances(Ants const& ants, double slope, double y_intercept,
                      std::vector<double>& average_distances)
{
  double total_distance =
      std::accumulate(ants.begin(), ants.end(), 0., [&](Ant& ant) {
        return (std::abs(slope * ant.getPosition().x + y_intercept
                         - ant.getPosition().y))
             / std::sqrt(slope * slope + 1);
      });
  double avg_distance{total_distance / ants.getNumberOfAnts()};
  average_distances.push_back(avg_distance);
}
} // namespace kape
