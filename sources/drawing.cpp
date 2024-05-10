#include "drawing.hpp"
#include "ants.hpp"
#include "geometry.hpp"
#include <stdexcept> //for std::invalid_argument, std::runtime_error

// TODO
//  - the implementation of the Window constructor is a bit bad
//  - find a solution for worldToScreen having to use static_cast
//  - remove pullAllEvents()

namespace kape {
CoordinateConverter::CoordinateConverter(float meter_to_pixels)
    : meter_to_pixels_{meter_to_pixels}
{
  if (meter_to_pixels_ <= 0) {
    throw std::invalid_argument{
        "1 meter can't equal a negative or null number of pixels"};
  }
}

float CoordinateConverter::getMeterToPixels() const
{
  return meter_to_pixels_;
}

void CoordinateConverter::setMeterToPixels(float meter_to_pixels)
{
  if (meter_to_pixels <= 0) {
    throw std::invalid_argument{
        "1 meter can't equal a negative or null number of pixels"};
  }

  meter_to_pixels_ = meter_to_pixels;
}

float CoordinateConverter::pixelsToMeters(float distance_in_pixels) const
{
  return distance_in_pixels / meter_to_pixels_;
}

float CoordinateConverter::metersToPixels(float distance_in_meters) const
{
  return distance_in_meters * meter_to_pixels_;
}

sf::Vector2f CoordinateConverter::worldToScreen(Vector2d const& world_position,
                                                unsigned int window_width,
                                                unsigned int window_height)
{
  sf::Vector2f res{static_cast<float>(world_position.x),
                   static_cast<float>(world_position.y)};
  res *= meter_to_pixels_; // scale to pixels
  res.y *= -1.0f;          // invert y axis
  res.x += static_cast<float>(window_width)
         / 2.f; // translate origin to the left edge of the screen
  res.y += static_cast<float>(window_height)
         / 2.f; // translate origin to the top edge of the screen

  return res;
}

// Window implementation---------------------------------------

Window::Window(unsigned int window_width, unsigned int window_height,
               float meter_to_pixel)
{
  window_.create(sf::VideoMode(window_width, window_height), "Project-KAPE",
                 sf::Style::Resize | sf::Style::Close);
  coord_conv_.setMeterToPixels(meter_to_pixel);

  if (!window_.isOpen()) {
    throw std::runtime_error{"failed to open the window"};
  }
}

bool Window::isOpen() const
{
  return window_.isOpen();
}

void Window::pullAllEvents() // BAD
{
  if (!isOpen()) {
    return;
  }

  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      close();
    }
    // catch the resize events
    else if (event.type == sf::Event::Resized) {
      // update the view to the new size of the window
      sf::FloatRect visible_area{0.f, 0.f, static_cast<float>(event.size.width),
                                 static_cast<float>(event.size.height)};
      window_.setView(sf::View(visible_area));
    } else if (event.type == sf::Event::MouseWheelScrolled) {
      float multiplier{event.mouseWheelScroll.delta < 0.f ? 0.8f : 1.2f};
      coord_conv_.setMeterToPixels(coord_conv_.getMeterToPixels() * multiplier);
    }
  }
}
void Window::clear(sf::Color const& color)
{
  if (isOpen()) {
    window_.clear(color);
  }
}
// to be perfected, it's just to see something on the screen right now
void Window::draw(Ant const& ant)
{
  if (!isOpen()) {
    return;
  }

  sf::CircleShape ant_drawing{coord_conv_.metersToPixels(0.01f)};
  ant_drawing.setOrigin({ant_drawing.getRadius(), ant_drawing.getRadius()});
  ant_drawing.setPosition(coord_conv_.worldToScreen(
      ant.getPosition(), window_.getSize().x, window_.getSize().y));
  window_.draw(ant_drawing);
}
void Window::display()
{
  if (isOpen()) {
    window_.display();
  }
}
void Window::close()
{
  if (isOpen()) {
    window_.close();
  }
}

Window::~Window()
{
  close();
}
} // namespace kape