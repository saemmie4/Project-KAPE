#include "simulation.hpp"
#include <iostream>

int main()
{
  kape::Simulation sim;
  if (!sim.chooseAndLoadSimulation()) {
    std::cout << "[ERROR]: something went wrong loading the simulation, please "
                 "refer to the logs at ./log/log.txt\n";
    return 1;
  }

  sim.run();

  return 0;
}

// OGGI:
// - Relazione
// - Colori Anthill, Obstacles, Sfondo, Bottoni, Logo
// - Test
// - Mappe 
// - i for con algoritmi
// - Pheromones (??)
// - test iteratori (square)
// 
// 
// 