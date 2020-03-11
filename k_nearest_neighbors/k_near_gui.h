#ifndef K_NEAR_GUI_INCLUDED
#define K_NEAR_GUI_INCLUDED

#include "Harmony/sfml/gui/gui.h"

#include "k_nearest_function.h"

#include <array>
#include <vector>
#include <map>
#include <string>

#include <memory> // shared_ptr

enum undo_type { added, deleted, cleared };

template<typename T>
struct Undoable {
  undo_type what;
  std::vector<Point<T>> points;
  Undoable(undo_type w, const Point<T>& p): what(w), points(1, p) { }
  Undoable(undo_type w, const std::vector<Point<T>>& vec_p): what(w), points(vec_p) { }

  // Return string based text for undo / redo Text_Display based on Undoable's state
  std::string to_string() const;
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
  const sf::Color eggshell = sf::Color(240, 234, 214, 255); // eggshell
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

#endif
