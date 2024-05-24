#include "drawing.hpp"
#include "ants.hpp"
#include "geometry.hpp"
#include "logger.hpp"
#include <algorithm> //for transform
#include <stdexcept> //for std::invalid_argument, std::runtime_error
// TODO
//  - the implementation of the Window constructor is a bit bad
//  - loadAntAnimationFrames uses a for loop which is kinda bad

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

sf::Vector2f
CoordinateConverter::worldToScreen(Vector2d const& world_position,
                                   unsigned int window_width,
                                   unsigned int window_height) const
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
float CoordinateConverter::worldToScreenRotation(double angle) const
{
  // for SFML angle>0: clockwise, angle<0:anticlockwise.
  // t0 degrees are on the screen's negative y axis (-> 90 - ...)
  return static_cast<float>(90 - angle * 180. / PI);
}

// Window implementation---------------------------------------
void Window::loadForDrawing(Food const& food)
{
  unsigned int window_width{window_.getSize().x};
  unsigned int window_height{window_.getSize().y};

  std::transform(
      food.begin(), food.end(), std::back_inserter(points_vector_),
      [window_width, window_height, this](FoodParticle const& food_particle) {
        sf::Vertex food_drawing{coord_conv_.worldToScreen(
            food_particle.getPosition(), window_width, window_height)};
        food_drawing.color = sf::Color::Green;
        return food_drawing;
      });
}

void Window::loadForDrawing(Pheromones const& pheromones)
{
  sf::Color color{pheromones.getPheromonesType() == Pheromones::Type::TO_ANTHILL
                      ? sf::Color::Blue
                      : sf::Color::Red};

  unsigned int window_width{window_.getSize().x};
  unsigned int window_height{window_.getSize().y};

  std::transform(
      pheromones.begin(), pheromones.end(), std::back_inserter(points_vector_),
      [window_width, window_height, &color,
       this](PheromoneParticle const& pheromone_particle) {
        sf::Vertex pheromone_drawing{coord_conv_.worldToScreen(
            pheromone_particle.getPosition(), window_width, window_height)};
        color.a = static_cast<sf::Uint8>(
            (pheromone_particle.getIntensity() / 100. * 255.));
        pheromone_drawing.color = color;
        return pheromone_drawing;
      });
}

void Window::drawLoaded()
{
  if (!points_vector_.empty()) {
    window_.draw(points_vector_.data(), points_vector_.size(), sf::Points);
  }
}

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

// Note: the first frame, if frames_naming_convention is left as is, would be
// Ant_frame_0.png won't do anything if it fails to load from the path may throw
// std::invalid argument if "[X]" isn't part of frames_naming_convention
bool Window::loadAntAnimationFrames(
    std::string const& animation_frames_filepath,
    std::size_t number_of_animation_frames,
    std::string const& frames_naming_convention)
{
  std::string const string_to_be_substituted{"[X]"};

  std::size_t subtitute_position =
      frames_naming_convention.find(string_to_be_substituted);

  if (subtitute_position == frames_naming_convention.npos) {
    throw std::invalid_argument{
        "no \"[X]\" found in the string frames_naming_convention"};
  }
  std::string frame_name_prefix =
      frames_naming_convention.substr(0, subtitute_position);
  std::string frame_name_suffix = frames_naming_convention.substr(
      subtitute_position + string_to_be_substituted.size(),
      frames_naming_convention.size()
          - (subtitute_position + string_to_be_substituted.size()));

  ants_animation_frames_.reserve(number_of_animation_frames);

  try {
    for (std::size_t current_frame{0};
         current_frame < number_of_animation_frames; ++current_frame) {
      ants_animation_frames_.push_back(sf::Texture{});
      if (!ants_animation_frames_.back().loadFromFile(
              animation_frames_filepath + frame_name_prefix
              + std::to_string(current_frame) + frame_name_suffix)) {
        throw std::runtime_error{"failed to load Ant's frame number "
                                 + std::to_string(current_frame)};
      }
    }
  } catch (std::runtime_error const& error) {
    log << "[ERROR]: \tFrom Window::loadAntAnimationFrames(...): failed to "
           "load the ants textures."
        << "\n\t\t\tfilepath: " << animation_frames_filepath
        << "\n\t\t\tnaming convention used: " << frames_naming_convention
        << "\n\t\t\terror reported: " << error.what() << '\n';
    ants_animation_frames_.clear();
    return false;
  }

  return true;
}
void Window::clear(sf::Color const& color)
{
  if (isOpen()) {
    window_.clear(color);
    points_vector_.clear();
  }
}

void Window::draw(Circle const& circle, sf::Color const& color,
                  std::size_t point_count)
{
  if (!isOpen()) {
    return;
  }

  sf::CircleShape circle_drawing{
      coord_conv_.metersToPixels(circle.getCircleRadius()), point_count};
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

// to be perfected, it's just to see something on the screen right now
// may throw std::runtime_error if it tries to draw a frame that hasn't been
// previously loaded
void Window::draw(Ant const& ant)
{
  if (!isOpen()) {
    return;
  }

  std::size_t current_frame{static_cast<std::size_t>(ant.getCurrentFrame())};
  if (current_frame >= ants_animation_frames_.size()) {
    throw std::runtime_error{"tried to draw a frame [frame "
                             + std::to_string(current_frame)
                             + "] that hasn't been loaded"};
  }

  sf::Sprite ant_drawing;
  ant_drawing.setTexture(ants_animation_frames_.at(static_cast<std::size_t>(ant.getCurrentFrame())));
  ant_drawing.setOrigin(
      sf::Vector2f{ant_drawing.getTexture()->getSize().x / 2.f,
                   ant_drawing.getTexture()->getSize().y / 2.f});

  // scaling the ant_drawing to make it the correct size on the screen
  float ant_drawing_length{coord_conv_.metersToPixels(Ant::ANT_LENGTH)};
  float ant_drawing_width{ant_drawing_length / 2.f};
  sf::Vector2f scale_factor{
      ant_drawing_width / ant_drawing.getTexture()->getSize().x,
      ant_drawing_length / ant_drawing.getTexture()->getSize().y};
  ant_drawing.setScale(scale_factor);

  ant_drawing.setPosition(coord_conv_.worldToScreen(
      ant.getPosition(), window_.getSize().x, window_.getSize().y));

  ant_drawing.setRotation(
      coord_conv_.worldToScreenRotation(ant.getFacingAngle()));

  // ant_drawing.setColor((ant.hasFood() ? sf::Color::Green : sf::Color::White));

  window_.draw(ant_drawing);
}

void Window::draw(Ants const& ants)
{
  for (auto const& ant : ants) {
    draw(ant);
  }
}

void Window::draw(Anthill const& anthill)
{
  draw(Circle{anthill.getCenter(), anthill.getRadius()}, sf::Color::Yellow);
}

void Window::draw(Obstacles const& obstacles, sf::Color const& color)
{
  for (auto obstacle : obstacles) {
    draw(obstacle, color);
  }
}

void Window::draw(Food const& food, Pheromones const& pheromone1,
                  Pheromones const& pheromone2)
{
  loadForDrawing(food);
  loadForDrawing(pheromone1);
  loadForDrawing(pheromone2);

  drawLoaded();
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