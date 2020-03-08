#ifndef K_NEAR_GUI_INCLUDED
#define K_NEAR_GUI_INCLUDED

#include "Harmony/sfml/gui/gui.h"

#include "k_nearest_function.h"

#include <array>
#include <vector>
#include <map>
#include <string>

#include <memory> // shared_ptr

#include <sstream>
#include <iomanip>

const std::string Alphas = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // TO DO

enum undo_type { added, deleted, cleared };
template<typename T>
struct Undoable {
  undo_type what;
  std::vector<Point<T>> points;
  Undoable(undo_type w, Point<T> p): what(w), points(1, p) { }
  Undoable(undo_type w, std::vector<Point<T>> vec_p): what(w), points(vec_p) { }

  std::string to_string() {
    if constexpr(std::is_same<T, int>()) {
      switch(what) {
        case added:
          return "ADD " + Alphas.substr(points[0].y()-1, 1) + ":" + std::to_string(points[0].x());
          break;
        case deleted:
          return "DEL " + Alphas.substr(points[0].y()-1, 1) + ":" + std::to_string(points[0].x());
          break;
        case cleared:
          return "Cleared!";
          break;
      }
    }
    if constexpr(std::is_same<T, float>()) {
      std::stringstream out;
      switch(what) {
        case added:
          out << "ADD (" << std::fixed << std::setprecision(2) << points[0].y() / 100.f
              << ", " << points[0].x() / 100.f << ")";
          break;
        case deleted:
          out << "DEL (" << std::fixed << std::setprecision(2) << points[0].y() / 100.f
              << ", " << points[0].x() / 100.f << ")";
          break;
        case cleared:
          return "Cleared!";
          break;
      }
      return out.str();
    }
    return "";
  }
};

enum Mode { int_ = 0, float_, none_ };

struct Mode_Base {
  virtual ~Mode_Base() = default;
};

template<typename T>
struct Mode_State : public Mode_Base {
  Point<T> selected;
  std::vector<Point<T>> points;
  std::vector<Undoable<T>> undo_vec;
  int undo_pos = -1;
  ~Mode_State() override = default;
};

template <typename T>
std::shared_ptr<Mode_State<T>> state_cast(std::shared_ptr<Mode_Base> base) {
  return std::dynamic_pointer_cast<Mode_State<T>>(base);
}

// Class containing Gui & the Gui's interactions for k_nearest_neighbors project
class K_Near_Gui {
  // RHS GUI 
  mutable Gui gui;
  const sf::Color background = sf::Color(240, 234, 214, 255); // eggshell
  const sf::Color undo_view_color = sf::Color(133, 133, 133); // slight grey
  Toggle_Button int_button;
  Toggle_Button float_button;
  Text_Display k_value;
  Push_Button k_decrease;
  Push_Button k_increase;
  Text_Display popup;
  Toggle_Button add_mode;
  Toggle_Button del_mode;
  Toggle_Button sel_mode;
  Push_Button undo;
  Push_Button redo;
  Push_Button clear;
  std::array<Text_Display, 5> undo_view;

  // LHS GUI
  Gui int_lhs;
  const int box_size = 50;
  const int border_size = 1;
  std::vector<std::vector<bool>> active_positions;
  sf::RectangleShape int_box;
  Gui float_lhs;

  // State of gui buttons
  std::map<size_t, bool> gui_state;
  // stores k_nearest & radius values for drawing ( transfer info from update_mode to draw function )
  struct update_info {
    std::vector<int> k_near;
    float radius;
  } update_;

  // Update gui, passed state & neighbors, & calculate update_ for next 'draw' call 
  // params : window, current state based on mode, neighbors
  template<typename T>
  void update_mode(sf::RenderWindow&, std::shared_ptr<Mode_State<T>>, size_t&);
  template<typename T>
  void draw_mode(sf::RenderWindow&, std::shared_ptr<Mode_State<T>>, size_t) const;
public:
  // params : window, font, neighbors
  K_Near_Gui(sf::RenderWindow&, const sf::Font&, size_t);
  // Return current mode
  Mode mode() const;
  // updates gui_state & calls update_mode with correct template based on current mode
  // params : window, state_map, neighbors
  void update(sf::RenderWindow&, std::map<Mode, std::shared_ptr<Mode_Base>>&, size_t&);
  // draws correct gui & draws the state passed in
  // params : window, state_map, neighbors
  void draw(sf::RenderWindow&, const std::map<Mode, std::shared_ptr<Mode_Base>>&, size_t) const;
};
/*
template<typename T>
update_info update_state(std::shared_ptr<Mode_State<T>> state,
                         std::vector<std::vector<bool>>& active_positions,
                         size_t neighbors, Gui& gui, const std::array<Text_Display, 5>& undo_view) {
    // Get k nearest points indicies
    update_info ret{k_nearest(state->points, state->selected, neighbors)};
    // Calculate radius for k nearest points
    int rad = 0;
    for(auto val : ret.k_near) {
      auto dist = metric(state->points[val], state->selected);
      if(dist > rad) rad = dist;
    }
    ret.radius = rad;
    if(state->undo_pos + 1 < state->undo_vec.size())
      gui.set_text(undo_view[0](), state->undo_vec[state->undo_pos + 1].to_string());
    else  gui.set_text(undo_view[0](), "");
    for(int i = 0; i < 4; ++i) {
      gui.set_text(undo_view[i + 1](),
                   i <= state->undo_pos ? state->undo_vec[state->undo_pos - i].to_string() : "");
    }
    return ret;
}*/

#endif
