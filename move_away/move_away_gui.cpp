#include "move_away_gui.h"

Move_Away_Gui::Move_Away_Gui(sf::RenderWindow& window, const sf::Vector2u& window_size,
                             const sf::Font& font):
  rhs_gui_size(window_size.x - window_size.y, window_size.y),
  left_margin(rhs_gui_size.x / 10 + rhs_gui_size.y),
  button_width(rhs_gui_size.x * 4 / 5),
  gui(window, font),
  fps(70, 20, window_size.x - 60, 5, "--"),
  add_point(button_width, 100, left_margin, 200, "ADD"),
  poke(button_width, 100, left_margin, 310, "POKE"),
  add_vel_x(rhs_gui_size.x * 2 / 5, 100, left_margin, 450, "--", sf::Color(160, 160, 160)),
  add_vel_y(rhs_gui_size.x * 2 / 5, 100, left_margin, 580, "--", sf::Color(160, 160, 160)),
  point_charge(rhs_gui_size.x * 2 / 5, 100, left_margin, 710, "0.0", sf::Color(160, 160, 160)),
  vel_x_up(20, 45, left_margin + button_width / 2 + 5, 450, "^"),
  vel_x_down(20, 45, left_margin + button_width / 2 + 5, 505, "V"),
  vel_y_up(20, 45, left_margin + button_width / 2 + 5, 580, "^"),
  vel_y_down(20, 45, left_margin + button_width / 2 + 5, 635, "V"),
  charge_up(20, 45, left_margin + button_width / 2 + 5, 710, "^"),
  charge_down(20, 45, left_margin + button_width / 2 + 5, 765, "V"),
  vel_x_rand(rhs_gui_size.x * 2 / 5 - 30, 100, left_margin + button_width / 2 + 30, 450, "RAND"),
  vel_y_rand(rhs_gui_size.x * 2 / 5 - 30, 100, left_margin + button_width / 2 + 30, 580, "RAND"),
  charge_rand(rhs_gui_size.x * 2 / 5 - 30, 100, left_margin + button_width / 2 + 30, 710, "RAND") {

  gui.add_background(Background(rhs_gui_size.x, rhs_gui_size.y, rhs_gui_size.y, 0));
  // TO DO : Backgrounds drawing in wrong direction

  gui.add_text(fps);
  gui.add_linked_toggle(add_point, poke);
  gui.add_text(add_vel_x, Text_Display(button_width, 20, left_margin, 430, "VELOCITY X"),
               add_vel_y, Text_Display(button_width, 20, left_margin, 560, "VELOCITY Y"),
               point_charge, Text_Display(button_width, 20, left_margin, 690, "CHARGE"));
  gui.add_push_button(vel_x_up, vel_x_down, vel_y_up, vel_y_down, charge_up, charge_down);
  gui.add_toggle_button(vel_x_rand, vel_y_rand, charge_rand);
}

void Move_Away_Gui::update(const sf::Vector2u& window_size, std::vector<Point>& points) {

  // Update points
  for(const auto& p : points) for(auto& point : points) point.push_from(p);
  for(auto& point : points) point.update(window_size.y, window_size.y);

  // fps
  auto curr_time = std::chrono::steady_clock::now();
  gui.set_text(fps(),
      std::to_string(1.f / std::chrono::duration<float>(curr_time - last_time).count()).substr(0,4));
  last_time = curr_time;
}
