#ifndef CHARGED_PARTICLES_GUI_H
#define CHARGED_PARTICLES_GUI_H

#include "components.h"

#include <string>
#include <chrono>

// Holds all info & Gui Interface for dealing with 1 stat block ( out of vel_x, vel_y, charge, mass )
class Gui_Stat_Block {
  float value;
  float min_val, max_val;
  Text_Display display;
  Push_Button up;
  Push_Button down;
  Toggle_Button random;

public:
  // params : value, min, max value, y_position, rhs_width, left_margin (x_pos), gui, label
  Gui_Stat_Block(float, float, float, size_t, size_t, size_t, Gui&, const std::string&);

  float get_value() const { return value; }

  // If id passed is one of this's elements => updates value & respective gui element appropiately
  // Returns true if id was one of the updated gui elements in this class ( for short circuiting )
  bool active_id(size_t id, Gui&);
  // If id passed is one of this's elements => updates respective gui element appropiately
  // Returns true if id was one of the updated gui elements in this class ( for short circuiting )
  bool deactive_id(size_t id, Gui&);
};

// All Gui features for charged_particles app ( drawing and input actions )
class Charged_Particle_Gui {

  const sf::Vector2u rhs_gui_size;
  const size_t left_margin;
  const size_t button_width;

  Gui gui;
  
  Text_Display fps;
  std::chrono::time_point<std::chrono::steady_clock> last_time = std::chrono::steady_clock::now();

  Toggle_Button add_point;
  Toggle_Button del_point;
  Toggle_Button poke;
  Toggle_Button pause;

  Gui_Stat_Block vel_x_block;
  Gui_Stat_Block vel_y_block;
  Gui_Stat_Block mass_block;
  Gui_Stat_Block charge_block;

  bool clicked = false;
  std::chrono::time_point<std::chrono::steady_clock> last_click = std::chrono::steady_clock::now();
  std::chrono::milliseconds click_sensitive_dur = std::chrono::milliseconds(400);

public:

  Charged_Particle_Gui(sf::RenderWindow&, const sf::Vector2u&, const sf::Font&);
  // Updates 'points' & UI based on gui state, calls 'push_from' & 'update' on all points, & does fps
  void update(sf::RenderWindow&, std::vector<Point>& points);
  void draw(sf::RenderWindow& window) const { gui.draw(window); }
};

#endif
