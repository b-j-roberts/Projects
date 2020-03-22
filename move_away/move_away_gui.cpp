#include "move_away_gui.h"
#include <iostream>

#include <cstdlib> // rand
#include <limits> // numeric limits

// Function alias for chrono's duration_cast<milliseconds>
template <typename... Args>
auto duration_ms(Args&&... args) ->
  decltype(std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...)) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::forward<Args>(args)...);
}

Gui_Stat_Block::Gui_Stat_Block(float val, float min_v, float max_v, size_t y_pos,
                               const sf::Vector2u& rhs_gui_size, size_t left_margin,
                               Gui& gui, const std::string& label):
  value(val),
  min_val(min_v),
  max_val(max_v),
  display(rhs_gui_size.x * 2 / 5, 100, left_margin, y_pos, ""/*TO DO*/, sf::Color(160, 160, 160)),
  up(20, 45, left_margin + rhs_gui_size.x * 2 / 5 + 5, y_pos, "^"),
  down(20, 45, left_margin + rhs_gui_size.x * 2 / 5 + 5, y_pos + 55, "V"), 
  random(rhs_gui_size.x * 2 / 5 - 30, 100, left_margin + rhs_gui_size.x * 2 / 5 + 30, y_pos, "RAND") {
  gui.add_text(display, Text_Display(rhs_gui_size.x * 4 / 5, 20, left_margin, y_pos - 20, label));
  gui.add_push_button(up, down);
  gui.add_toggle_button(random);
}

bool Gui_Stat_Block::active_id(size_t id, Gui& gui) {
  if(id == up.id()) {
    value += (value < max_val) * .1f;
    return true;
  } else if(id == down.id()) {
    value -= (value > min_val) * .1f;
    return true;
  } else if(id == random.id()) { // Choose random value in range
    gui.set_text(display.id(), "RND");
    value = static_cast<float>((rand() % static_cast<int>((max_val - min_val) * 10 + 1)) + 
                               min_val * 10) / 10.f;
    return true;
  }
  return false;
}

bool Gui_Stat_Block::deactive_id(size_t id, Gui& gui) {
  if(id == random.id()) {
    gui.set_text(display.id(), std::to_string(value).substr(0,3 + (value < 0)));
    return true;
  }
  return false;
}

Move_Away_Gui::Move_Away_Gui(sf::RenderWindow& window, const sf::Vector2u& window_size,
                             const sf::Font& font):
  rhs_gui_size(window_size.x - window_size.y, window_size.y),
  left_margin(rhs_gui_size.x / 10 + rhs_gui_size.y),
  button_width(rhs_gui_size.x * 4 / 5),
  gui(window, font),
  fps(70, 20, window_size.x - 60, 5, "--"),
  add_point(button_width, 100, left_margin, 90, "ADD"),
  del_point(button_width, 100, left_margin, 200, "DEL"),
  poke(button_width, 100, left_margin, 310, "POKE"),
  pause(button_width / 4, 50, left_margin, 10, "||"),
  vel_x_block(0.f, -3.f, 3.f, 450, rhs_gui_size, left_margin, gui, "VELOCITY X"),
  vel_y_block(0.f, -3.f, 3.f, 580, rhs_gui_size, left_margin, gui, "VELOCITY Y"),
  mass_block(1.f, 1.f, 3.f, 710, rhs_gui_size, left_margin, gui, "MASS"),
  charge_block(0.f, -3.f, 3.f, 840, rhs_gui_size, left_margin, gui, "CHARGE") {
  // TO DO : Backgrounds drawing in wrong direction
  gui.add_background(Background(rhs_gui_size.x, rhs_gui_size.y, rhs_gui_size.y, 0));
  gui.add_text(fps);
  gui.add_linked_toggle(add_point, del_point, poke);
  gui.add_toggle_button(pause);
}

void Move_Away_Gui::update(sf::RenderWindow& window, std::vector<Point>& points) {

  const sf::Vector2u& window_size = window.getSize(); // TO DO : Error on resize?

  auto state = gui.get_state();
  
  Point poke_point;
  for(const auto& [id, val] : state) {
    if(val) { // GUI - Code interation if state is true

      if(id == add_point.id() && sf::Mouse::isButtonPressed(sf::Mouse::Left)) { 
        if(clicked) { // TO DO
          if(duration_ms(std::chrono::steady_clock::now() - last_click) > click_sensitive_dur) {
            clicked = false;
          }
        } else {
          clicked = true;
          last_click = std::chrono::steady_clock::now();
          const auto& click = sf::Mouse::getPosition(window);
          // Add point if in bounds
          if(click.x > 0 && static_cast<size_t>(click.x) < window_size.y && 
             click.y > 0 && static_cast<size_t>(click.y) < window_size.y) {
            points.emplace_back(click.x, click.y, vel_x_block.get_value(), vel_y_block.get_value(), 
                                mass_block.get_value(), charge_block.get_value());
          }
        }
      }
      else if(id == del_point.id() && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // TO DO : Drag delete?
        if(clicked) {
          if(duration_ms(std::chrono::steady_clock::now() - last_click) > click_sensitive_dur) {
            clicked = false;
          }
        } else {
          clicked = true;
          last_click = std::chrono::steady_clock::now();
          if(points.empty()) continue;
          const auto& click = sf::Mouse::getPosition(window);
          // Compute minimum square distance to a Point in 'points' and its position
          auto sq = [](auto val){ return val * val; };
          auto sq_dist = [&](const Point& p){ return sq(p.x() - click.x) + sq(p.y() - click.y); };
          float min_sq_dist = std::numeric_limits<float>::max();
          long closest_pos = 0, current_pos = 0;
          for(const auto& p : points) {
            float test_dist = sq_dist(p);
            if(test_dist < min_sq_dist) {
              min_sq_dist = test_dist;
              closest_pos = current_pos;
            }
            ++current_pos;
          }
          // If minimum square distance is less than minimum square radius then delete that point
          if(min_sq_dist < sq(10)) { // TO DO
            points.erase(points.begin() + closest_pos, points.begin() + closest_pos + 1);
          }
        }
      }
      else if(id == poke.id() && sf::Mouse::isButtonPressed(sf::Mouse::Left)) { 
        if(state[pause.id()]) continue;
        const auto& click = sf::Mouse::getPosition(window);
        poke_point = Point(click.x, click.y, 0, 0, mass_block.get_value(), charge_block.get_value());
      }
      else if(id == pause.id()) {
        // TO DO : I want to do anything?
      }
      else if(vel_x_block.active_id(id, gui)) { }
      else if(vel_y_block.active_id(id, gui)) { }
      else if(mass_block.active_id(id, gui)) { }
      else if(charge_block.active_id(id, gui)) { }

    } else { // GUI - Code interation if state is false
      if(vel_x_block.deactive_id(id, gui)) { }
      else if(vel_y_block.deactive_id(id, gui)) { }
      else if(mass_block.deactive_id(id, gui)) { }
      else if(charge_block.deactive_id(id, gui)) { }
    }
  }

  // Update points
  if(!state[pause.id()]) {
    for(const auto& p : points) for(auto& point : points) point.push_from(p);
    if(state[poke.id()]) { for(auto& point : points) point.push_from(poke_point); }
    for(auto& point : points) point.update(window_size.y, window_size.y);
  }

  // fps
  auto curr_time = std::chrono::steady_clock::now();
  gui.set_text(fps(),
      std::to_string(1.f / std::chrono::duration<float>(curr_time - last_time).count()).substr(0,4));
  last_time = curr_time;
}
