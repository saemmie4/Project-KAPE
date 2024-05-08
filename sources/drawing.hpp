#ifndef DRAWING_HPP
#define DRAWING_HPP
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "geometry.hpp"

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
  double getPixelToMeter() const;
  // may throw std::invalid_argument if pixel_to_meter <= 0
  void setPixelToMeter(double meter_to_pixels) const;
  double pixelToMeters(double distance_in_pixels) const;
  double meterToPixels(double distance_in_meters) const;

  sf::Vector2<double> worldToScreen (Vector2d const& world_position, int window_width, int window_height);
};

// name to be changed
class Renderer
{
 private:
  sf::RenderWindow window_;
};
} // namespace kape

#endif