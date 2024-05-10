#ifndef DRAWING_HPP
#define DRAWING_HPP
#include "ants.hpp"
#include "geometry.hpp"
#include <SFML/Graphics.hpp>

// TODO
//  - find a solution for worldToScreen having to make a static_cast
//  double->float
//  - remove pullAllEvents()

namespace kape {
// right now we'll assume that the origin of the map lies at the center of the
// screen, and that +x = right, +y = up
class CoordinateConverter
{
 private:
  float meter_to_pixels_;

 public:
  // may throw std::invalid_argument if meter_to_pixels <= 0
  CoordinateConverter(float meter_to_pixels = 1000.f);
  float getMeterToPixels() const;
  // may throw std::invalid_argument if pixel_to_meter <= 0
  void setMeterToPixels(float meter_to_pixels);
  float pixelsToMeters(float distance_in_pixels) const;
  float metersToPixels(float distance_in_meters) const;

  sf::Vector2f worldToScreen(Vector2d const& world_position,
                             unsigned int window_width,
                             unsigned int window_height);
};

// name to be changed
class Window
{
 private:

 public:
  sf::RenderWindow window_;
  CoordinateConverter coord_conv_;
  // may throw std::invalid_argument if meter_to_pixel <= 0
  // may throw std::runtime_error if it fails to open a new window
  explicit Window(unsigned int window_width, unsigned int window_height,
                  float meter_to_pixel = 1000.f);
  bool isOpen() const;
  void pullAllEvents(); // BAD
  void clear(sf::Color const& color);
  void draw(Ant const& ant);
  void display();
  void close();

  ~Window();
};
} // namespace kape

#endif