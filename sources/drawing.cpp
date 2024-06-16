#include "drawing.hpp"
#include "ants.hpp"
#include "geometry.hpp"
#include "logger.hpp"
#include <algorithm> //for transform
#include <cassert>
#include <cmath> //for std::round
#include <iostream>
#include <stdexcept> //for std::invalid_argument, std::runtime_error
// TODO
//  - the implementation of the Window constructor is a bit bad
//  - loadAntAnimationFrames uses a for loop which is kinda bad
//  - draw buttons glitches if text is too long

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
void Window::loadForDrawing(Food const& food, sf::Color const& food_color)
{
  unsigned int window_width{window_.getSize().x};
  unsigned int window_height{window_.getSize().y};

  std::transform(
      food.begin(), food.end(), std::back_inserter(points_vector_),
      [window_width, window_height, &food_color, this](FoodParticle const& food_particle) {
        sf::Vertex food_drawing{coord_conv_.worldToScreen(
            food_particle.getPosition(), window_width, window_height)};
        food_drawing.color = food_color;
        return food_drawing;
      });
}

void Window::loadForDrawing(Pheromones const& pheromones, sf::Color const& pheromones_color)
{
  unsigned int window_width{window_.getSize().x};
  unsigned int window_height{window_.getSize().y};

  renderInto(
      points_vector_, pheromones,
      [window_width, window_height, this](
          PheromoneParticle const& pheromone_particle, sf::Vector2f& position) {
        position = coord_conv_.worldToScreen(pheromone_particle.getPosition(),
                                             window_width, window_height);
      }, pheromones_color);

  // std::transform(
  //     pheromones.begin(), pheromones.end(),
  //     std::back_inserter(points_vector_), [window_width, window_height,
  //     &color, max_pheromone_intensity,
  //      this](PheromoneParticle const& pheromone_particle) {
  //       sf::Vertex pheromone_drawing{coord_conv_.worldToScreen(
  //           pheromone_particle.getPosition(), window_width, window_height)};
  //       color.a = static_cast<sf::Uint8>(
  //           (pheromone_particle.getIntensity() / max_pheromone_intensity *
  //           255.));
  //       pheromone_drawing.color = color;
  //       return pheromone_drawing;
  //     });
}

void Window::drawLoaded()
{
  draw(points_vector_);
}
void Window::createWindow()
{
  if (isOpen()) {
    close();
  }

  window_.create(sf::VideoMode::getDesktopMode(), "Project-KAPE",
                 sf::Style::Fullscreen);
  is_fullscreen_ = true;
}

void Window::createWindow(unsigned int window_width, unsigned int window_height)
{
  if (isOpen()) {
    close();
  }

  window_.create(sf::VideoMode(window_width, window_height), "Project-KAPE",
                 sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

  is_fullscreen_ = false;
}

void Window::draw(sf::RectangleShape const& rectangle, sf::Text text,
                  sf::Color const& rectangle_color)
{
  sf::RectangleShape rectangle_drawing{rectangle};
  rectangle_drawing.setFillColor(rectangle_color);
  window_.draw(rectangle_drawing);

  sf::Vector2f center = {text.getGlobalBounds().width / 2.f,
                         text.getGlobalBounds().height / 2.f};
  sf::Vector2f localBounds =
      center
      + sf::Vector2f{text.getLocalBounds().left, text.getLocalBounds().top};
  sf::Vector2f rounded{std::round(localBounds.x), std::round(localBounds.y)};
  text.setOrigin(rounded);

  text.setPosition(
      rectangle_drawing.getPosition().x + rectangle_drawing.getSize().x / 2.f,
      rectangle_drawing.getPosition().y + rectangle_drawing.getSize().y / 2.f);

  // std::size_t num_char{text.getString().getSize()};
  // size_t * size_t or size_t * uint becomes a float on its own, the static
  // cast is to avoid the warning
  if (text.getGlobalBounds().width > 0.9f * rectangle_drawing.getSize().x) {
    text.setScale(
        0.9f * rectangle_drawing.getSize().x / text.getGlobalBounds().width,
        0.9f * rectangle_drawing.getSize().x / text.getGlobalBounds().width);
    // text.setCharacterSize(static_cast<unsigned int>(
    //     static_cast<size_t>(rectangle_drawing.getSize().x) * 9 / 10
    //     / text.getString().getSize()));
  }

  window_.draw(text);
}

sf::RectangleShape
Window::kapeRectangleToScreenSfRectangleShape(Rectangle const& rectangle) const
{
  sf::RectangleShape rectangle_drawing{
      sf::Vector2f(coord_conv_.metersToPixels(rectangle.getRectangleWidth()),
                   coord_conv_.metersToPixels(rectangle.getRectangleHeight()))};

  rectangle_drawing.setPosition(
      coord_conv_.worldToScreen(rectangle.getRectangleTopLeftCorner(),
                                window_.getSize().x, window_.getSize().y));
  return rectangle_drawing;
}

Window::Window(float meter_to_pixel)
{
  createWindow();
  coord_conv_.setMeterToPixels(meter_to_pixel);

  if (!font_.loadFromFile(DEFAULT_FONT_FILEPATH)) {
    throw std::runtime_error{"From Window::Window(float meter_to_pixel) "
                             "could not find the font at \""
                             + DEFAULT_FONT_FILEPATH + "\""};
  }
  if (!window_.isOpen()) {
    throw std::runtime_error{
        "[ERROR]: from Window::Window(float meter_to_pixel):"
        "\n\t\t\tFailed to open the window "};
  }
}

Window::Window(unsigned int window_width, unsigned int window_height,
               float meter_to_pixel)
{
  createWindow(window_width, window_height);
  coord_conv_.setMeterToPixels(meter_to_pixel);

  if (!font_.loadFromFile(DEFAULT_FONT_FILEPATH)) {
    throw std::runtime_error{"From Window::Window(float meter_to_pixel) "
                             "could not find the font at \""
                             + DEFAULT_FONT_FILEPATH + "\""};
  }
  if (!window_.isOpen()) {
    throw std::runtime_error{
        "[ERROR]: from Window::Window(unsigned int window_width, unsigned int "
        "window_height, float meter_to_pixel):"
        "\n\t\t\tFailed to open the window "};
  }
}
sf::Font const& Window::getFont() const
{
  return font_;
}

float Window::getMeterToPixels() const
{
  return coord_conv_.getMeterToPixels();
}
// may throw std::invalid_argument if pixel_to_meter <= 0
void Window::setMeterToPixels(float meter_to_pixels)
{
  coord_conv_.setMeterToPixels(meter_to_pixels);
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
    switch (event.type) {
    case sf::Event::Closed:
      close();
      break;
    case sf::Event::Resized: // update the view to the new size of the window
    {
      sf::FloatRect visible_area{0.f, 0.f, static_cast<float>(event.size.width),
                                 static_cast<float>(event.size.height)};
      window_.setView(sf::View(visible_area));
    } break;
    case sf::Event::MouseWheelScrolled: // zoom / dezoom
    {
      float delta{event.mouseWheelScroll.delta};
      // MacBooks send 0.f if the scolling is very small both in the zoom and
      // unzoom direction, and therefore we can't know if we should zoom or
      // not
      if (delta != 0.f) {
        float multiplier{delta < 0.f ? 0.8f : 1.2f};
        coord_conv_.setMeterToPixels(coord_conv_.getMeterToPixels()
                                     * multiplier);
      }
    } break;

    case sf::Event::KeyReleased:
      switch (event.key.code) {
      case sf::Keyboard::Escape:
        if (is_fullscreen_) {
          createWindow(sf::VideoMode::getDesktopMode().width,
                       sf::VideoMode::getDesktopMode().height);
        }
        break;
      case sf::Keyboard::F11:
        if (is_fullscreen_) {
          createWindow(sf::VideoMode::getDesktopMode().width,
                       sf::VideoMode::getDesktopMode().height);
        } else {
          createWindow();
        }
        break;

      default:
        break;
      }
      break;

    default:
      break;
    }
  }
}

// Note: the first frame, if frames_naming_convention is left as is, would be
// Ant_frame_0.png won't do anything if it fails to load from the path may
// throw std::invalid argument if "[X]" isn't part of frames_naming_convention
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
      kapeRectangleToScreenSfRectangleShape(rectangle)};
  rectangle_drawing.setFillColor(color);
  window_.draw(rectangle_drawing);
}
void Window::draw(Rectangle const& rectangle, sf::Text text,
                  sf::Color const& rectangle_color)
{
  draw(kapeRectangleToScreenSfRectangleShape(rectangle), text, rectangle_color);
}

// to be perfected, it's just to see something on the screen right now
// may throw std::runtime_error if it tries to draw a frame that hasn't been
// previously loaded
void Window::draw(Ant const& ant, bool debug_mode)
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
  ant_drawing.setTexture(ants_animation_frames_.at(current_frame));
  float texture_width{
      static_cast<float>(ant_drawing.getTexture()->getSize().x)};
  float texture_height{
      static_cast<float>(ant_drawing.getTexture()->getSize().y)};
  ant_drawing.setOrigin(
      sf::Vector2f{texture_width / 2.f, texture_height / 2.f});

  // scaling the ant_drawing to make it the correct size on the screen
  float ant_drawing_length{coord_conv_.metersToPixels(Ant::ANT_LENGTH)};
  float ant_drawing_width{ant_drawing_length / 2.f};
  sf::Vector2f scale_factor{ant_drawing_width / texture_width,
                            ant_drawing_length / texture_height};
  ant_drawing.setScale(scale_factor);

  ant_drawing.setPosition(coord_conv_.worldToScreen(
      ant.getPosition(), window_.getSize().x, window_.getSize().y));

  ant_drawing.setRotation(
      coord_conv_.worldToScreenRotation(ant.getFacingAngle()));

  window_.draw(ant_drawing);

  if (debug_mode) {
    // render the circles of vision
    std::array<kape::Circle, 3> circles_of_vision;
    ant.calculateCirclesOfVision(circles_of_vision);
    draw(circles_of_vision[0], sf::Color::Blue);
    draw(circles_of_vision[1], sf::Color::Blue);
    draw(circles_of_vision[2], sf::Color::Blue);

    // render in green the desired direction, in red the current direction
    std::array<sf::Vertex, 4> direction_lines;
    direction_lines[0] =
        sf::Vertex(ant_drawing.getPosition(), sf::Color::Green);
    direction_lines[1] =
        sf::Vertex(coord_conv_.worldToScreen(
                       ant.getPosition()
                           + 4. * Ant::ANT_LENGTH * ant.getDesiredDirection(),
                       window_.getSize().x, window_.getSize().y),
                   sf::Color::Green);

    direction_lines[2] = sf::Vertex(ant_drawing.getPosition(), sf::Color::Red);
    direction_lines[3] = sf::Vertex(
        coord_conv_.worldToScreen(ant.getPosition()
                                      + 4. * Ant::ANT_LENGTH * ant.getVelocity()
                                            / norm(ant.getVelocity()),
                                  window_.getSize().x, window_.getSize().y),
        sf::Color::Red);

    window_.draw(direction_lines.data(), direction_lines.size(),
                 sf::LinesStrip);
  }
}

void Window::draw(Ants const& ants, bool debug_mode)
{
  for (auto const& ant : ants) {
    draw(ant, debug_mode);
  }
}

void Window::draw(Anthill const& anthill, sf::Color const& color)
{
  draw(Circle{anthill.getCenter(), anthill.getRadius()}, color);

  double circle_r{anthill.getRadius()};
  Rectangle text_box{anthill.getCenter()
                         + circle_r * Vector2d{-sqrt(3.) / 2., 0.5},
                     sqrt(3.) * circle_r, circle_r};
  sf::Text food_counter;
  food_counter.setCharacterSize(30);
  food_counter.setFillColor(sf::Color::White);
  food_counter.setFont(font_);
  food_counter.setString(std::to_string(anthill.getFoodCounter()));
  draw(text_box, food_counter, color);
}

void Window::draw(Obstacles const& obstacles, sf::Color const& color)
{
  for (auto obstacle : obstacles) {
    draw(obstacle, color);
  }
}

void Window::draw(Food const& food, Pheromones const& to_anthill_pheromones,
          Pheromones const& to_food_pheromones, sf::Color const& food_color,
          sf::Color const& to_anthill_pheromones_color,
          sf::Color const& to_food_pheromones_color)
{
  loadForDrawing(food, food_color);
  loadForDrawing(to_anthill_pheromones, to_anthill_pheromones_color);
  loadForDrawing(to_food_pheromones, to_food_pheromones_color);

  drawLoaded();
}

void Window::draw(std::vector<sf::Vertex> const& points)
{
  if (!points.empty()) {
    window_.draw(points.data(), points.size(), sf::Points);
  }
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

// may throw std::runtime_error if the window is not open when the function is
// called
std::size_t Window::chooseOneOption(std::vector<std::string> const& options)
{
  if (!isOpen()) {
    throw std::runtime_error{
        "called Window::chooseOneOption(std::vector<std::string> const& "
        "options) "
        "while the window is not open"};
  };

  struct RectangleText{
    sf::RectangleShape box;
    sf::Text text;
  };

  float window_width{static_cast<float>(window_.getSize().x)};
  float window_height{static_cast<float>(window_.getSize().y)};
  float button_spacing{window_height / static_cast<float>(options.size() + 1)};
  std::vector<RectangleText> buttons;

  int rectangle_index{0};
  for (auto const& option : options) {
    sf::RectangleShape rect{
        sf::Vector2f{3.f * button_spacing, button_spacing * 0.8f}};
    rect.setPosition(sf::Vector2f{0.01f * window_width,
                                  (static_cast<float>(rectangle_index) + 0.5f)
                                      * button_spacing});

    sf::Text text;
    text.setFont(font_);
    text.setString(option);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    buttons.push_back(RectangleText{rect, text});
    ++rectangle_index;
  }

  bool chosen{false};
  std::size_t chosen_option{0};
  while (!chosen) {
    sf::Event event;
    while (window_.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Right:
        case sf::Keyboard::Down:
          if (chosen_option >= buttons.size() - 1) {
            chosen_option = 0;
          } else {
            ++chosen_option;
          }
          break;
        case sf::Keyboard::Left:
        case sf::Keyboard::Up:
          if (chosen_option == 0) {
            chosen_option = options.size() - 1;
          } else {
            --chosen_option;
          }
          break;
        case sf::Keyboard::Enter:
          chosen = true;
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    }

    
    clear(sf::Color::Black);
    std::size_t index{0};
    for (auto const& button : buttons) {
      draw(button.box, button.text,
           index == chosen_option ? sf::Color::Green : sf::Color::Red);
      ++index;
    }
    display();
  }

  assert(chosen == true);
  assert(chosen_option < options.size());
  return chosen_option;
}

Window::~Window()
{
  close();
}

void graphPoints(std::vector<double> const& points)
{
  if (points.empty()) {
    return;
  }

  try {
    Window window{};
    window.setMeterToPixels(334.f);
    double max_y{std::abs(*std::max_element(
        points.begin(), points.end(), [](double largest, double rhs) {
          return std::abs(rhs) > std::abs(largest);
        }))};
    double max_x{static_cast<double>(points.size() - 1)};

    double x_offset = -1.;
    double y_offset = -1.;

    std::vector<Circle> points_drawing;
    Rectangle x_axis{Vector2d{x_offset, y_offset}, 1.5, 0.01};
    Rectangle y_axis{Vector2d{x_offset - 0.01, 1.5 + y_offset}, 0.01, 1.51};

    Rectangle x_axis_title_box{
        Vector2d{x_offset + 1.5 - 0.06, y_offset + 0.05 + 0.098}, 0.8, 0.4};
    Rectangle y_axis_title_box{
        Vector2d{x_offset - 0.745, y_offset + 1.5 + 0.23}, 0.8, 0.4};
    sf::Text x_axis_title;
    sf::Text y_axis_title;
    x_axis_title.setFont(window.getFont());
    x_axis_title.setString(">\nIndice del tempo");
    x_axis_title.setCharacterSize(400);
    x_axis_title.setFillColor(sf::Color::Black);
    y_axis_title.setFont(window.getFont());
    y_axis_title.setString("Indice di distanza\n                  A");
    y_axis_title.setCharacterSize(400);
    y_axis_title.setFillColor(sf::Color::Black);

    double x{0.};
    for (auto y : points) {
      points_drawing.emplace_back(
          Vector2d{x / max_x + x_offset, y / max_y + y_offset},
          1. / (2 * max_x));
      ++x;
    }

    while (window.isOpen()) {
      window.inputHandling();

      window.clear(sf::Color::White);
      for (auto const& point_drawing : points_drawing) {
        window.draw(point_drawing, sf::Color::Black);
      }

      window.draw(x_axis_title_box, x_axis_title, sf::Color::White);
      window.draw(y_axis_title_box, y_axis_title, sf::Color::White);
      window.draw(x_axis, sf::Color::Black);
      window.draw(y_axis, sf::Color::Black);
      window.display();
    }

  } catch (std::runtime_error& error) {
    log << error.what();
    std::cout
        << "[INFO]: Failed to open the window to display the results of the "
           "optimization.\n\t\t\tIn lack of a better option, the points will "
           "be saved in ./log/log.txt\n";
    int index{0};
    for (auto point : points) {
      std::cout << "(" << index << ", " << point << ")\n";
      ++index;
    }
    return;
  }
}

} // namespace kape