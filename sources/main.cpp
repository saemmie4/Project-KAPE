#include "ants.hpp"
#include "drawing.hpp"
#include <SFML/Graphics.hpp>

#include <iostream>

int main()
{
  kape::Ant ant{{0., 0.}, {0., 0.}};
  kape::Circle circle{{0.02, 0.}, 0.01};
  kape::Rectangle rectangle{{0., 0.02}, 0.01, 0.02};
  kape::Window window{700u, 600u};

  while (window.isOpen()) {
    window.clear(sf::Color::Red);
    window.draw(ant);
    window.draw(circle, sf::Color::Blue);
    window.draw(rectangle, sf::Color::Green);
    window.display();
    window.pullAllEvents();
  }

  return 0;
}