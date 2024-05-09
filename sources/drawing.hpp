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
  double meter_to_pixels_;

 public:
  // may throw std::invalid_argument if meter_to_pixels <= 0
  CoordinateConverter(double meter_to_pixels);
  double getMeterToPixels() const;
  // may throw std::invalid_argument if pixel_to_meter <= 0
  void setMeterToPixels(double meter_to_pixels);
  double pixelsToMeters(double distance_in_pixels) const;
  double metersToPixels(double distance_in_meters) const;

  // may throw std::invalid_argument if window_width or window_height are <= 0
  sf::Vector2f worldToScreen(Vector2d const& world_position, int window_width,
                             int window_height);
};

// name to be changed
class Renderer
{
 private:
  sf::RenderWindow window_;
  CoordinateConverter coord_conv_;

 public:
  // may throw std::invalid_argument if any if the parameters are <= 0
  // may throw std::runtime_error if it fails to open a new window
  explicit Renderer(int window_width, int window_height,
                    double meter_to_pixel = 1000);
  bool isOpen() const;
  void pullAllEvents(); // BAD
  void clear(sf::Color const& color);
  void draw(Ant const& ant);
  void display();
  void close();

  ~Renderer();
};
} // namespace kape

#endif