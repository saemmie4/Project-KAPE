#include "ants.hpp"
#include "drawing.hpp"
#include <SFML/Graphics.hpp>

#include <iostream>

int main()
{
  kape::Ant ant{{0., 0.}, {0., 0.}};
  kape::Window window{700u, 600u};

  while (window.isOpen()) {
    window.clear(sf::Color::Red);
    window.draw(ant);
    window.display();
    window.pullAllEvents();
  }

  return 0;
}