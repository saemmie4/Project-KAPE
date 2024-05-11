#ifndef SIMULATION_HPP
#define SIMULATION_HPP

// TODO:
//  -constructor
//  -deconstructor
//  -run()

#include "ants.hpp"
#include "environment.hpp"
#include <vector>

namespace kape {
class Simulation
{
 private:
  Pheromones to_anthill_;
  Pheromones to_food_;
  Food food_;
  Obstacles obstacles_
  std::vector<Ant> ants_;

 public:
  explicit Simulation();
  void run();
};
} // namespace kape

#endif