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

double CoordinateConverter::pixelsToMeters(float distance_in_pixels) const
{
  return distance_in_pixels / meter_to_pixels_;
}

float CoordinateConverter::metersToPixels(double distance_in_meters) const
{
  return static_cast<float>(distance_in_meters) * meter_to_pixels_;
}

sf::Vector2f CoordinateConverter::worldToScreen(Vector2d const& world_position,
                                                unsigned int window_width,
                                                unsigned int window_height)
{
  sf::Vector2f res{static_cast<float>(world_position.x),
                   static_cast<float>(world_position.y)};
  res *= meter_to_pixels_; // scale to pixels
  res.y *= -1.0f;          // invert y axis

  // Translation of the origin
  // sum vector from top left (screen origin) to middle of the screen (world
  // origin)
  res += sf::Vector2f{static_cast<float>(window_width) / 2.f,
                      static_cast<float>(window_height) / 2.f};

  return res;
}

// note: angle is in radians, the returned angle is in degrees
float CoordinateConverter::worldToScreenRotation(double angle)
{
  // for SFML angle>0: clockwise, angle<0:anticlockwise.
  // t0 degrees are on the screen's negative y axis (-> 90 - ...)
  return static_cast<float>(90 - angle * 180. / PI);
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

void Window::inputHandling()
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
      float delta{event.mouseWheelScroll.delta};
      // MacBooks send 0.f if the scolling is very small both in the zoom and
      // unzoom direction, and therefore we can't know if we should zoom or not
      if (delta != 0.f) {
        float multiplier{delta < 0.f ? 0.8f : 1.2f};
        coord_conv_.setMeterToPixels(coord_conv_.getMeterToPixels()
                                     * multiplier);
      }
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

  float length{coord_conv_.metersToPixels(Ant::ANT_LENGTH)};
  sf::RectangleShape ant_drawing{sf::Vector2f{length / 3.f, length}};
  ant_drawing.setOrigin(sf::Vector2f(ant_drawing.getSize().x / 2.f,
                                     ant_drawing.getSize().y / 2.f));

  ant_drawing.setPosition(coord_conv_.worldToScreen(
      ant.getPosition(), window_.getSize().x, window_.getSize().y));

  ant_drawing.setRotation(coord_conv_.worldToScreenRotation(ant.getFacingAngle()));
  window_.draw(ant_drawing);
}

void Window::draw(Circle const& circle, sf::Color const& color)
{
  if (!isOpen()) {
    return;
  }

  sf::CircleShape circle_drawing{
      coord_conv_.metersToPixels(circle.getCircleRadius())};
  circle_drawing.setOrigin(
      sf::Vector2f(circle_drawing.getRadius(), circle_drawing.getRadius()));
  circle_drawing.setPosition(coord_conv_.worldToScreen(
      circle.getCircleCenter(), window_.getSize().x, window_.getSize().y));

  circle_drawing.setFillColor(color);
  window_.draw(circle_drawing);
}
void Window::draw(Rectangle const& rectangle, sf::Color const& color)
{
  sf::RectangleShape rectangle_drawing{
      sf::Vector2f(coord_conv_.metersToPixels(rectangle.getRectangleWidth()),
                   coord_conv_.metersToPixels(rectangle.getRectangleHeight()))};

  rectangle_drawing.setPosition(
      coord_conv_.worldToScreen(rectangle.getRectangleTopLeftCorner(),
                                window_.getSize().x, window_.getSize().y));

  rectangle_drawing.setFillColor(color);
  window_.draw(rectangle_drawing);
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