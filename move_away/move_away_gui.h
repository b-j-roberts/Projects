#ifndef MOVE_AWAY_GUI_H
#define MOVE_AWAY_GUI_H

#include "Harmony/sfml/gui/gui.h"

#include "Components.h"

#include <chrono>

#include <string>

class Gui_Stat_Block {
  float value;
  float min_val, max_val;
  Text_Display display;
  Push_Button up;
  Push_Button down;
  Toggle_Button random;

public:
  Gui_Stat_Block(float, float, float, size_t, const sf::Vector2u&, size_t, Gui&, const std::string&);

  float get_value() const { return value; }

  bool active_id(size_t id, Gui&);
  bool deactive_id(size_t id, Gui&);
};

class Move_Away_Gui {

  const sf::Vector2u rhs_gui_size;
  const size_t left_margin;
  const size_t button_width;

  Gui gui;
  
  Text_Display fps;

  Toggle_Button add_point;
  Toggle_Button del_point;
  Toggle_Button poke;
  Toggle_Button pause;

  Gui_Stat_Block vel_x_block;
  Gui_Stat_Block vel_y_block;
  Gui_Stat_Block mass_block;
  Gui_Stat_Block charge_block;

  std::chrono::time_point<std::chrono::steady_clock> last_time = std::chrono::steady_clock::now();
  bool clicked = false;
  std::chrono::time_point<std::chrono::steady_clock> last_click = std::chrono::steady_clock::now();
  std::chrono::milliseconds click_sensitive_dur = std::chrono::milliseconds(400);

public:

  Move_Away_Gui(sf::RenderWindow&, const sf::Vector2u&, const sf::Font&);
  void update(sf::RenderWindow&, std::vector<Point>& points);
  void draw(sf::RenderWindow& window) const { gui.draw(window); }
};

#endif
