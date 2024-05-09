#include "drawing.hpp"
#include "ants.hpp"
#include "geometry.hpp"
#include <stdexcept> //for std::invalid_argument, std::runtime_error

// TODO
//  - the implementation of the Renderer constructor is bad
//  - find a solution for worldToScreen having to make a static_cast
//  double->float
//  - remove pullAllEvents()

namespace kape {
CoordinateConverter::CoordinateConverter(double meter_to_pixels)
    : meter_to_pixels_{meter_to_pixels}
{
  if (meter_to_pixels_ <= 0) {
    throw std::invalid_argument{
        "1 meter can't equal a negative or null number of pixels"};
  }
}

double CoordinateConverter::getMeterToPixels() const
{
  return meter_to_pixels_;
}

void CoordinateConverter::setMeterToPixels(double meter_to_pixels)
{
  if (meter_to_pixels <= 0) {
    throw std::invalid_argument{
        "1 meter can't equal a negative or null number of pixels"};
  }

  meter_to_pixels_ = meter_to_pixels;
}

double CoordinateConverter::pixelsToMeters(double distance_in_pixels) const
{
  return distance_in_pixels / meter_to_pixels_;
}

double CoordinateConverter::metersToPixels(double distance_in_meters) const
{
  return distance_in_meters * meter_to_pixels_;
}

sf::Vector2f CoordinateConverter::worldToScreen(Vector2d const& world_position,
                                                int window_width,
                                                int window_height)
{
  if (window_width <= 0) {
    throw std::invalid_argument{"The window's width can't be negative or null"};
  }
  if (window_height <= 0) {
    throw std::invalid_argument{
        "The window's height can't be negative or null"};
  }

  // this stuff is bad

  sf::Vector2f res{static_cast<float>(world_position.x),
                   static_cast<float>(world_position.y)};
  res *= static_cast<float>(meter_to_pixels_); // scale to pixels
  res.y *= -1.0f;                              // invert y axis
  res.x += static_cast<float>(window_width)
         / 2.f; // translate origin to the left edge of the screen
  res.y += static_cast<float>(window_height)
         / 2.f; // translate origin to the top edge of the screen

  return res;
}

// Renderer implementation---------------------------------------

//BADBADBAD
Renderer::Renderer(int window_width, int window_height, double meter_to_pixel)
    : window_{sf::RenderWindow(sf::VideoMode(window_width, window_height),
                               "Project-KAPE",
                               sf::Style::Resize | sf::Style::Close)}
    , coord_conv_{CoordinateConverter(meter_to_pixel)}
{
  if (!window_.isOpen()) {
    throw std::runtime_error{"failed to open the window"};
  }
}

bool Renderer::isOpen() const
{
  return window_.isOpen();
}

void Renderer::pullAllEvents() // BAD
{
  if (!isOpen()) {
    return;
  }

  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      close();
    }
  }
}

void Renderer::clear(sf::Color const& color)
{
  if (isOpen()) {
    window_.clear(color);
  }
}
// to be perfected, it's just to see something on the screen right now
void Renderer::draw(Ant const& ant)
{
  if (!isOpen()) {
    return;
  }

  sf::CircleShape ant_drawing{5.f};
  ant_drawing.setOrigin({ant_drawing.getRadius(), ant_drawing.getRadius()});
  ant_drawing.setPosition(coord_conv_.worldToScreen(
      ant.getPosition(), window_.getSize().x, window_.getSize().y));
  window_.draw(ant_drawing);
}
void Renderer::display()
{
  if (isOpen()) {
    window_.display();
  }
}
void Renderer::close()
{
  if (isOpen()) {
    window_.close();
  }
}

Renderer::~Renderer()
{
  close();
}
} // namespace kape