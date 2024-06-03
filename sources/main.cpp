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
  kape::Window window{700u, 600u, 1000.f};
  window.loadAntAnimationFrames("./assets/ants/",
                                kape::Ant::ANIMATION_TOTAL_NUMBER_OF_FRAMES);

  kape::Anthill anthill{};
  kape::Ants ants{};
  kape::Pheromones ph_anthill{kape::Pheromones::Type::TO_ANTHILL};
  kape::Pheromones ph_food{kape::Pheromones::Type::TO_FOOD};
  kape::Obstacles obs{};
  kape::Food food{};

  anthill.loadFromFile();
  ants.loadFromFile(anthill);
  obs.loadFromFile();
  food.loadFromFile(obs);

  std::vector<long long int> t_count;

  while (window.isOpen()) {
    auto start{std::chrono::high_resolution_clock::now()};

    ants.update(food, ph_anthill, ph_food, anthill, obs, 0.01);
    ph_anthill.updateParticlesEvaporation(0.01);
    ph_food.updateParticlesEvaporation(0.01);

    window.clear(sf::Color(184, 139, 74));

    // window.loadForDrawing(ph_anthill);
    // window.loadForDrawing(ph_food);

    // window.loadForDrawing(food);
    // window.drawLoaded();

    window.draw(food, ph_anthill, ph_food);

    window.draw(ants);

    for (auto const& ant : ants) {
      std::array<kape::Circle, 3> circles_of_vision;
      ant.calculateCirclesOfVision(circles_of_vision);
      window.draw(circles_of_vision[0], sf::Color::Blue);
      window.draw(circles_of_vision[1], sf::Color::Blue);
      window.draw(circles_of_vision[2], sf::Color::Blue);
    }
    window.draw(anthill);

    window.draw(obs, sf::Color::Yellow);

    window.display();

    window.inputHandling();

    t_count.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                          .count());
  }
  std::cout << ph_anthill.getNumberOfPheromones()
                   + ph_food.getNumberOfPheromones()
            << '\n';

  std::cout << std::accumulate(t_count.begin(), t_count.end(), 0.)
                   / t_count.size()
            << "\n";

  return 0;
}