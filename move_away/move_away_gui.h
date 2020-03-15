#ifndef MOVE_AWAY_GUI_H
#define MOVE_AWAY_GUI_H

#include "Harmony/sfml/gui/gui.h"

#include "Components.h"

#include <chrono>

#include <string>

class Move_Away_Gui {

  const sf::Vector2i rhs_gui_size;
  const size_t left_margin;
  const size_t button_width;

  Gui gui;
  
  Text_Display fps;

  Toggle_Button add_point;
  Toggle_Button poke;

  Text_Display add_vel_x;
  Text_Display add_vel_y;
  Text_Display point_charge;

  Push_Button vel_x_up;
  Push_Button vel_x_down;
  Push_Button vel_y_up;
  Push_Button vel_y_down;
  Push_Button charge_up;
  Push_Button charge_down;

  Toggle_Button vel_x_rand;
  Toggle_Button vel_y_rand;
  Toggle_Button charge_rand;

  std::chrono::time_point<std::chrono::steady_clock> last_time = std::chrono::steady_clock::now();

public:

  Move_Away_Gui(sf::RenderWindow&, const sf::Vector2u&, const sf::Font&);
  void update(const sf::Vector2u&, std::vector<Point>& points);
  void draw(sf::RenderWindow& window) const { gui.draw(window); }
};

#endif
