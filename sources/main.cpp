#include "ants.hpp"
#include "drawing.hpp"
#include <SFML/Graphics.hpp>

// for testing, to be removed
#include <chrono>
#include <cmath>

int main()
{
  kape::Ant ant{{0., 0.}, {-1., 1.}};
  kape::Circle circle{{0., 0.}, 0.1};
  kape::Rectangle rectangle{{0., 0.}, 0.01, 0.2};
  kape::Window window{700u, 600u};

  auto start_time{std::chrono::high_resolution_clock::now()};

  while (window.isOpen()) {
    std::chrono::duration<double> duration{
        std::chrono::duration_cast<std::chrono::milliseconds>(
            (std::chrono::high_resolution_clock::now() - start_time))};
    double seconds_passed{duration.count()};

    circle.setCircleCenter(kape::Vector2d{0.1 * cos(3. * seconds_passed),
                                          0.1 * sin(2. * seconds_passed)});
    rectangle.setRectangleTopLeftCorner(
        kape::Vector2d{0.1 * cos(seconds_passed), 0.2 * sin(4.*seconds_passed) });

    window.clear(sf::Color::Black);
    // window.draw(ant);
    sf::Color col =
        (kape::doShapesIntersect(circle, rectangle) ? sf::Color::Red
                                                    : sf::Color::Green);
    window.draw(ant);
    window.draw(circle, col);
    window.draw(rectangle, col);
    window.display();
    window.inputHandling();
  }

  return 0;
}