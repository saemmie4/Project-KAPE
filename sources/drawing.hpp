#ifndef DRAWING_HPP
#define DRAWING_HPP
#include "ants.hpp"
#include "geometry.hpp"
#include <SFML/Graphics.hpp>

// TODO
//  - find a solution for worldToScreen having to make a static_cast
//  double->float

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
  double pixelsToMeters(float distance_in_pixels) const;
  float metersToPixels(double distance_in_meters) const;

  sf::Vector2f worldToScreen(Vector2d const& world_position,
                             unsigned int window_width,
                             unsigned int window_height) const;

  // note: angle is in radians, the returned angle is in degrees
  float worldToScreenRotation(double angle) const;
};

// name to be changed
class Window
{
 private:
  sf::RenderWindow window_;
  CoordinateConverter coord_conv_;

  // holds the points of Food and Pheromones to be drawn on the next
  // display() call gets cleared with clear()
  std::vector<sf::Vertex> points_vector_;

void loadForDrawing(Food const& food);
  void loadForDrawing(Pheromones const& pheromones);
  void drawLoaded();
 public:
  // may throw std::invalid_argument if meter_to_pixel <= 0
  // may throw std::runtime_error if it fails to open a new window
  explicit Window(unsigned int window_width, unsigned int window_height,
                  float meter_to_pixel = 1000.f);
  bool isOpen() const;
  void inputHandling();
  void clear(sf::Color const& color);
  void draw(Circle const& circle, sf::Color const& color,
            std::size_t point_count = 30U);
  void draw(Rectangle const& rectangle, sf::Color const& color);
  void draw(Ant const& ant);
  void draw(Ants const& ants);
  void draw(Anthill const& anthill);
  void draw(Obstacles const& obstacles, sf::Color const& color);
  void draw(Food const& food, Pheromones const& pheromone1, Pheromones const& pheromone2);
  void display();
  void close();

  ~Window();
};
} // namespace kape

#endif