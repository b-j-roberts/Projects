#ifndef K_NEAR_GUI_INCLUDED
#define K_NEAR_GUI_INCLUDED

#include "Harmony/sfml/gui/gui.h"

#include "k_nearest_function.h"

#include <array>
#include <vector>
#include <map>
#include <string>

#include <memory> // shared_ptr

// Explicit Instantiation Declaration to supress clang warning
extern template int metric<int>(const Point<int>& p1, const Point<int>& p2); 
extern template float metric<float>(const Point<float>& p1, const Point<float>& p2);

// type of operation that the Undoable is storing
enum undo_type { added, deleted, cleared };

// Holds a single undoable operation's info, ie type & points added/deleted
template<typename T>
struct Undoable {
  undo_type what;
  std::vector<Point<T>> points;
  // For added & deleted type ( only 1 point )
  Undoable(undo_type w, const Point<T>& p): what(w), points(1, p) { }
  // For cleared type ( vector of points )
  Undoable(undo_type w, const std::vector<Point<T>>& vec_p): what(w), points(vec_p) { }

  // Return string based text for undo / redo Text_Display based on Undoable's state
  std::string to_string() const;
};

enum Mode { int_ = 0, float_, none_ };

// Base class used to hold Mode_States in a pointer
// TO DO : Think about using variants and visitors instead ( bc only 2 possible states int & float )
struct Mode_Base {
  virtual ~Mode_Base() = default;
};

// Holds the state of a Mode
template<typename T>
struct Mode_State : public Mode_Base {
  Point<T> selected;
  std::vector<Point<T>> points;
  std::vector<Undoable<T>> undo_vec;
  int undo_pos = -1;
  ~Mode_State() override = default;
};

// Alias used for template dynamic_pointer_cast of mode state ( downcasting Mode_Base to Mode_State )
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
  const int box_size = 50; // TO DO
  const int border_size = 1;
  std::vector<std::vector<bool>> active_positions;
  mutable sf::RectangleShape int_box;
  Gui float_lhs;

  // State of gui buttons
  std::map<size_t, bool> gui_state;
  // stores k_nearest & radius values for drawing ( transfer info from update to draw function )
  struct update_info {
    std::vector<size_t> k_near;
    float radius;
  } update_;

  // Update gui, passed state & neighbors, & calculate update_ for next 'draw' call 
  // params : window, current state based on mode, neighbors
  template<typename T>
  void update_mode(sf::RenderWindow&, std::shared_ptr<Mode_State<T>>, size_t&);
  // Draw LHS of the GUI based on mode & update_ from update_mode
  // params : window, current state based on mode, neighbors
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
  // draws gui to window based on current mode & calls draw_mode with correct template
  // params : window, state_map, neighbors
  void draw(sf::RenderWindow&, const std::map<Mode, std::shared_ptr<Mode_Base>>&, size_t) const;
};

#endif
