#ifndef DRAWING_HPP
#define DRAWING_HPP
#include "ants.hpp"
#include "environment.hpp"
#include "geometry.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

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
  std::vector<sf::Texture> ants_animation_frames_;
  sf::Font font_;

  bool is_fullscreen_;

  // holds the points of Food and Pheromones to be drawn on the next
  // display() call gets cleared with clear()
  std::vector<sf::Vertex> points_vector_;

  void loadForDrawing(Food const& food);
  void loadForDrawing(Pheromones const& pheromones);
  void drawLoaded();
  // creates/recreates the window, making it fullscreen
  void createWindow();
  // creates/recreates the window with the specified dimensions
  void createWindow(unsigned int window_width, unsigned int window_height);
  void draw(sf::RectangleShape const& rectangle, sf::Text text,
            sf::Color const& rectangle_color);
  sf::RectangleShape
  kapeRectangleToScreenSfRectangleShape(Rectangle const& rectangle) const;

 public:
  inline static std::string const DEFAULT_FONT_FILEPATH{
      "assets/font/courier-prime.regular.ttf"};
  // creates the window with size as big as possible
  // may throw std::invalid_argument if meter_to_pixel <= 0
  // may throw std::runtime_error if it fails to open a new window
  explicit Window(float meter_to_pixel = 1000.f);
  // may throw std::invalid_argument if meter_to_pixel <= 0
  // may throw std::runtime_error if it fails to open a new window
  explicit Window(unsigned int window_width, unsigned int window_height,
                  float meter_to_pixel = 1000.f);
  sf::Font const& getFont() const;
  float getMeterToPixels() const;
  // may throw std::invalid_argument if pixel_to_meter <= 0
  void setMeterToPixels(float meter_to_pixels);
  bool isOpen() const;
  void inputHandling();
  // Note: the first frame, if frames_naming_convention is left as is, would be
  // Ant_frame_0.png won't do anything if it fails to load from the path may
  // throw std::invalid argument if "[X]" isn't part of frames_naming_convention
  bool loadAntAnimationFrames(
      std::string const& animation_frames_filepath,
      std::size_t number_of_animation_frames,
      std::string const& frames_naming_convention = "Ant_frame_[X].png");
  void clear(sf::Color const& color);
  void draw(Circle const& circle, sf::Color const& color,
            std::size_t point_count = 30U);
  void draw(Rectangle const& rectangle, sf::Color const& color);
  void draw(Rectangle const& rectangle, sf::Text text,
            sf::Color const& rectangle_color);
  void draw(Ant const& ant);
  void draw(Ants const& ants);
  void draw(Anthill const& anthill);
  void draw(Obstacles const& obstacles, sf::Color const& color);
  void draw(Food const& food, Pheromones const& pheromone1,
            Pheromones const& pheromone2);
  void draw(std::vector<sf::Vertex> const& points);
  void display();
  void close();

  // may throw std::runtime_error if the window is not open when the function is
  // called
  std::size_t chooseOneOption(std::vector<std::string> const& options);

  ~Window();
};

// x is the index, y is the value
void graphPoints(std::vector<double> const& points);
} // namespace kape

#endif