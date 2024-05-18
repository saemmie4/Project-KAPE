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
  kape::Ant ant{{0., 0.}, {0.001, 0.005}};
  std::array<kape::Circle, 3> circles_of_vision;
  kape::Window window{700u, 600u};

  kape::Anthill anthill{{0., 0.}, 0.01};
  kape::Pheromones ph_anthill{kape::Pheromones::Type::TO_ANTHILL};
  kape::Pheromones ph_food{kape::Pheromones::Type::TO_FOOD};
  kape::Obstacles obs{};
  kape::Food food{};

  food.generateFoodInCircle({{0., 0.2}, 0.1}, 200, obs);
  food.generateFoodInCircle({{-0.2, 0.}, 0.1}, 200, obs);
  food.generateFoodInCircle({{0.2, 0.}, 0.1}, 200, obs);
  food.generateFoodInCircle({{0., -0.2}, 0.1}, 200, obs);

  obs.addObstacle({{-0.2, 0.15}, 0.4, 0.04});
  std::default_random_engine engine{417325ul};
  std::vector<int> t_count;

  while (window.isOpen()) {
    ant.calculateCirclesOfVision(circles_of_vision);
    ant.update(food, ph_anthill, ph_food, anthill, obs, engine);
    ph_anthill.updateParticlesEvaporation();
    ph_food.updateParticlesEvaporation();

    window.clear(sf::Color::Black);

    auto start{std::chrono::high_resolution_clock::now()};

    window.loadForDrawing(ph_anthill);
    window.loadForDrawing(ph_food);

    t_count.push_back(std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                          .count());
    window.loadForDrawing(food);
    window.drawLoaded();

    window.draw(ant);

    window.draw(circles_of_vision[0], sf::Color::Blue);
    window.draw(circles_of_vision[1], sf::Color::Blue);
    window.draw(circles_of_vision[2], sf::Color::Blue);

    window.draw(anthill);

    window.draw({{-0.2, 0.15}, 0.4, 0.04}, sf::Color::Yellow);

    window.display();

    window.inputHandling();
  }

  std::cout << std::accumulate(t_count.begin(), t_count.end(), 0.)
                   / t_count.size()
            << "\n";

  return 0;
}