#include "ants.hpp"
#include "drawing.hpp"
#include <SFML/Graphics.hpp>

// for testing, to be removed
#include <chrono>
#include <cmath>
#include <array>

int main()
{
  kape::Ant ant{{0., 0.}, {0.0, 0.005}};
  std::array<kape::Circle, 3> circles_of_vision;
  kape::Window window{700u, 600u};

  kape::Anthill anthill{{0., 0.}, 0.01};
  kape::Pheromones ph_anthill{kape::Pheromones::Type::TO_ANTHILL};
  kape::Pheromones ph_food{kape::Pheromones::Type::TO_FOOD};
  kape::Obstacles obs{};
  kape::Food food{};

  food.addFoodParticle({0., 0.1});
  food.addFoodParticle({0.01, 0.1});
  food.addFoodParticle({-0.01, -0.1});
  food.addFoodParticle({0.0, -0.1});

  while (window.isOpen()) {
    ant.calculateCirclesOfVision(circles_of_vision);
    ant.update(food, ph_anthill, ph_food, anthill, obs);
    ph_anthill.updateParticlesEvaporation();
    ph_food.updateParticlesEvaporation();

    window.clear(sf::Color::Black);


    window.draw(ph_anthill);
    window.draw(ph_food);

    // window.draw(ant);
    window.draw(circles_of_vision[0], sf::Color::Blue);
    window.draw(circles_of_vision[1], sf::Color::Blue);
    window.draw(circles_of_vision[2], sf::Color::Blue);
    
    window.draw(anthill);
    window.draw(food);

    window.display();
    window.inputHandling();
  }

  return 0;
}