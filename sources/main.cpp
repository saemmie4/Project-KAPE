#include <SFML/Graphics.hpp>
#include "ants.hpp"
#include "drawing.hpp"
int main()
{

  kape::Ant ant{{0.5,0.}, {0.,0.}};
  kape::Renderer rend{700, 200};
  while(rend.isOpen()){
    rend.clear(sf::Color::Black);
    rend.draw(ant);
    rend.display();
    rend.pullAllEvents();
  }


  return 0;
}