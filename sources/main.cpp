#include "ants.hpp"
#include "drawing.hpp"
#include <SFML/Graphics.hpp>

// for testing, to be removed
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>

int main()
{
  kape::Window window{700u, 600u, 500.f};

  kape::Anthill anthill{};
  kape::Ants ants{};
  kape::Pheromones ph_anthill{kape::Pheromones::Type::TO_ANTHILL};
  kape::Pheromones ph_food{kape::Pheromones::Type::TO_FOOD};
  kape::Obstacles obs{};
  kape::Food food{};

  
  anthill.loadFromFile();
  ants.addAntsAroundCircle(anthill.getCircle(), 50);
  obs.loadFromFile();
  food.loadFromFile(obs);

  std::vector<long long int> t_count;

  while (window.isOpen()) {
    ants.update(food, ph_anthill, ph_food, anthill, obs);
    ph_anthill.updateParticlesEvaporation();
    ph_food.updateParticlesEvaporation();

    window.clear(sf::Color::Black);

    auto start{std::chrono::high_resolution_clock::now()};
    // window.loadForDrawing(ph_anthill);
    // window.loadForDrawing(ph_food);

    // window.loadForDrawing(food);
    // window.drawLoaded();

    window.draw(food, ph_anthill, ph_food);

    t_count.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                          .count());
    window.draw(ants);

    // for (auto const& ant : ants) {
    //   std::array<kape::Circle, 3> circles_of_vision;
    //   ant.calculateCirclesOfVision(circles_of_vision);
    //   window.draw(circles_of_vision[0], sf::Color::Blue);
    //   window.draw(circles_of_vision[1], sf::Color::Blue);
    //   window.draw(circles_of_vision[2], sf::Color::Blue);
    // }
    window.draw(anthill);

    window.draw(obs, sf::Color::Yellow);

    window.display();

    window.inputHandling();
  }

  std::cout << std::accumulate(t_count.begin(), t_count.end(), 0.)
                   / t_count.size()
            << "\n";

  return 0;
}