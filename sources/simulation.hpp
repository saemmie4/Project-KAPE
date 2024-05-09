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
  Environment env;
  std::vector<Ant> ants;

 public:
  explicit Simulation();
  void run();
};
} // namespace kape

#endif