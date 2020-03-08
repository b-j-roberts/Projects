//#include "k_near_gui.h"
#include "Harmony/sfml/gui/gui.h"

#include <string>
#include <vector>
#include <array>
#include <memory> // shared_ptr
#include <math.h>
#include <algorithm> // min
#include <stdexcept>

#include <sstream>
#include <iomanip>

// TO DO : Labels & type detection
// TO DO : weight algorithms, ...
template<typename T, typename L = std::string>
class Point {

  T x_;
  T y_;

  const L label_;

public:

  Point(): x_(), y_() { }
  Point(T x, T y): x_(x), y_(y) { }
  Point(T x, T y, L label): x_(x), y_(y), label_(label) { }

  Point(const Point& p) { x_ = p.x_; y_ = p.y_; }
  Point& operator= (const Point& p) { x_ = p.x_; y_ = p.y_; }

  const T x() const { return x_; }
  const T y() const { return y_; } 
  const L& label() const { return label_; }

  void move(T x, T y) { x_ += x; y_ += y; };

  template<typename U>
  friend bool operator==(const Point<U>& p_1, const Point<U>& p_2);
};

template<typename T>
bool operator==(const Point<T>& p_1, const Point<T>& p_2) {
  return p_1.x_ == p_2.x_ && p_1.y_ == p_2.y_;
}

// METRICS

// Metrics return a const value of the distance between the two point objects given
template<typename T>
const T metric(const Point<T>& p1, const Point<T>& p2);

// City Block Metric
template<>
const int metric<int>(const Point<int>& p1, const Point<int>& p2) {
  return abs( p1.x() - p2.x() ) + abs( p1.y() - p2.y() );
}

// Euclidean Metric
template<>
const float metric<float>(const Point<float>& p1, const Point<float>& p2) {
  return sqrt( pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2) );
}


// K-NEAREST FUNCTIONALITY

// Holds position and value of element of a vector
template<typename T>
struct pos_holder {
  T value;
  size_t pos;
  pos_holder(T val, int i): value(val), pos(i) { }
};

// k_nearest returns a vector of the index of the k nearest points to point in points
template<typename T>
const std::vector<int> k_nearest(const std::vector<Point<T>>& points, 
                                 const Point<T>& point, size_t k) {  
  // Get distances between all points and passed point
  std::vector<T> dist(points.size());
  std::transform(points.begin(), points.end(), dist.begin(), 
                 [&](auto& p) { return metric(point, p); });

  // Group distance with vector position then sort by distance
  std::vector<pos_holder<T>> pos;
  std::transform(dist.begin(), dist.end(), std::back_inserter(pos),
                 [i = 0](auto elem) mutable { return pos_holder<T>(elem, i++); });

  // OPTIMIZE : Partial sort?
  std::sort(pos.begin(), pos.end(), [&](const pos_holder<T>& a,const pos_holder<T>& b) -> bool {
    return a.value < b.value;
  });

  // Take k closest values and return the index of them
  std::vector<int> ret;
  std::transform(pos.begin(), pos.begin() + std::min(k, pos.size()), std::back_inserter(ret),
                 [](const auto& holder){ return holder.pos; });
  return ret;
}

// Same as other k_nearest (see above) but for a point inside of the vector at position idx
template<typename T>
const std::vector<int> k_nearest(const std::vector<Point<T>>& points, size_t idx, size_t k) {
   auto ret = k_nearest(points, points[idx], k + 1);
   ret.erase(std::remove(ret.begin(), ret.end(), idx), ret.end());
   return ret;
}


// UNDO FEATURES


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

// MODES

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


template<typename T>
bool within_range(const sf::Vector2i& position, std::vector<std::vector<bool>>& active_positions) {
  if(std::is_same<T, int>()) {
    if(position.x < active_positions[0].size() && position.x > 0 &&
       position.y < active_positions.size() && position.y > 0) {
      return true;
    } else { return false; }
  } else if(std::is_same<T, float>()) {
    if(position.x > 0 && position.x < 1000 && // TO DO : Think about hardcoded size here
       position.y > 0 && position.y < 1000) {
      return true;
    } else { return false; }
  } else { return false; }
}

// Add point in state.selected and in active_positions ( if mode is int ) at position
template<typename T>
void add_op(std::shared_ptr<Mode_State<T>> state, const sf::Vector2i& position,
            std::vector<std::vector<bool>>& active_positions) {
  if(within_range<T>(position, active_positions)) {
    if constexpr(std::is_same<T, int>()) {
      if(active_positions[position.y][position.x]) return;
      else active_positions[position.y][position.x] = true;
    }
    state->points.emplace_back(position.x, position.y);
    state->undo_vec.erase(state->undo_vec.begin() + state->undo_pos + 1, state->undo_vec.end());
    state->undo_vec.emplace_back(added, *(state->points.end() - 1));
    ++(state->undo_pos);
  }
}

// Delete point in state.selected and in active_positions (i if mode is int ) at position
template<typename T>
void del_op(std::shared_ptr<Mode_State<T>> state, const sf::Vector2i& position,
            std::vector<std::vector<bool>>& active_positions) {
  if(within_range<T>(position, active_positions)) {
    if constexpr(std::is_same<T, int>()) {
      if(!active_positions[position.y][position.x]) return;
      else active_positions[position.y][position.x] = false;
    }
    state->undo_vec.erase(state->undo_vec.begin() + state->undo_pos + 1, state->undo_vec.end());
    if constexpr(std::is_same<T, int>()) {
      state->undo_vec.emplace_back(deleted, Point<int>(position.x, position.y));
      ++(state->undo_pos);
      state->points.erase(std::remove(state->points.begin(), state->points.end(), 
                                      Point<int>(position.x, position.y)), state->points.end());
    } else {
      Point<float> del_pos = Point<float>(position.x, position.y);
      auto idx_nearest = k_nearest<float>(state->points, del_pos, 1);
      if(!state->points.empty() && metric(del_pos, state->points[idx_nearest[0]]) < 8) {
        state->undo_vec.emplace_back(deleted, *(state->points.begin()+idx_nearest[0]));
        ++(state->undo_pos);
        state->points.erase(state->points.begin()+idx_nearest[0], 
                            state->points.begin()+idx_nearest[0] + 1);
      }
    }
  }
}

template<typename T>
void sel_op(std::shared_ptr<Mode_State<T>> state, const sf::Vector2i& position,
            std::vector<std::vector<bool>>& active_positions) {
  if(within_range<T>(position, active_positions)) {
    state->selected = Point<T>(position.x, position.y);
  }
}

template<typename T>
void clr_op(std::shared_ptr<Mode_State<T>> state, std::vector<std::vector<bool>>& active_positions) {
  state->undo_vec.erase(state->undo_vec.begin() + state->undo_pos + 1, state->undo_vec.end());
  state->undo_vec.emplace_back(cleared, state->points);
  ++(state->undo_pos);
  state->points.clear();
  if constexpr(std::is_same<T, int>())
    for(auto& rows : active_positions) std::fill(rows.begin(), rows.end(), false);
}

template<typename T>
void undo_op(std::shared_ptr<Mode_State<T>> state, std::vector<std::vector<bool>>& active_positions) {
  if(state->undo_pos > -1) {
    switch(state->undo_vec[state->undo_pos].what) {
      case added:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos].points[0].y()]
                          [state->undo_vec[state->undo_pos].points[0].x()] = false;
        state->points.erase(state->points.end() - 1, state->points.end());
        break;
      case deleted:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos].points[0].y()]
                          [state->undo_vec[state->undo_pos].points[0].x()] = true;
        state->points.push_back(state->undo_vec[state->undo_pos].points[0]);
        break;
      case cleared:
        state->points = state->undo_vec[state->undo_pos].points;
        if constexpr(std::is_same<T, int>())
          for(const auto& point : state->points) active_positions[point.y()][point.x()] = true;
        break;
    }
    --(state->undo_pos);
  }
}
template<typename T>
void redo_op(std::shared_ptr<Mode_State<T>> state, std::vector<std::vector<bool>>& active_positions) {
  if(state->undo_pos + 1 < state->undo_vec.size()) {
    switch(state->undo_vec[state->undo_pos + 1].what) {
      case added:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos + 1].points[0].y()]
                          [state->undo_vec[state->undo_pos + 1].points[0].x()] = true;
        state->points.push_back(state->undo_vec[state->undo_pos + 1].points[0]);
        break;
      case deleted:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos + 1].points[0].y()]
                          [state->undo_vec[state->undo_pos + 1].points[0].x()] = false;
        state->points.erase(state->points.end() - 1, state->points.end());
        break;
      case cleared:
        state->points.clear();
        if constexpr(std::is_same<T, int>())
          for(auto& rows : active_positions) std::fill(rows.begin(), rows.end(), false);
        break;
    }
    ++(state->undo_pos);
  }
}

struct update_info {
  std::vector<int> k_near;
  float radius;
};

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
}

template<typename T>
void draw_state(sf::RenderWindow& window, std::shared_ptr<Mode_State<T>> state, 
                const std::vector<std::vector<bool>>& active_positions, const update_info& update, 
                size_t neighbors, Gui& gui, const Text_Display& popup) {
  if constexpr(std::is_same<T, int>()) {
    const size_t box_size = 50, border_size = 1; // TO DO 
    sf::RectangleShape int_box(sf::Vector2f(box_size - 2 * border_size, box_size - 2 * border_size));
    int_box.setOutlineColor(sf::Color::Yellow);
    size_t border_count = 0, inside_count = 0;
    // Draw Grid // TO DO : Yellow outine instead of yellow box?
    for(int i = 1; i < active_positions.size(); ++i) {
      for(int j = 1; j < active_positions[i].size(); ++j) {
        int_box.setPosition(j * box_size + border_size, i * box_size + border_size);
        active_positions[i][j] ? int_box.setFillColor(sf::Color::Green) :
                                 int_box.setFillColor(sf::Color::Red);
        int dist = metric(Point(j, i), state->selected);
        if(dist == update.radius) {// TO DO : Fill in option?
          int_box.setFillColor(active_positions[i][j] ? sf::Color::Blue : sf::Color::Yellow);
          if(active_positions[i][j]) ++border_count;
        } else if(dist < update.radius) {
          if(active_positions[i][j]) ++inside_count;
        }
        window.draw(int_box);
      }
    }
    // Draw k nearest points
    for(auto val : update.k_near) {
      int_box.setPosition(state->points[val].x() * box_size + border_size,
                          state->points[val].y() * box_size + border_size);
      int_box.setFillColor(sf::Color::Blue);
      window.draw(int_box);
    }
    if(inside_count + border_count > neighbors) {
      gui.set_text(popup(), std::to_string(inside_count + border_count) +
                            " points highlighted inside radius because " +
                            std::to_string(border_count) + " points share the same max distance");
    } else {
      gui.set_text(popup(), "");
    }
    // Draw Selected Element ( fixes border issues ) // TO DO : Stop circle before moved
    if(state->selected.x() > 0 && state->selected.x() < active_positions[0].size() &&
       state->selected.y() > 0 && state->selected.y() < active_positions.size()) {
      int_box.setPosition(state->selected.x() * box_size + border_size,
                          state->selected.y() * box_size + border_size);
      active_positions[state->selected.y()][state->selected.x()] ?
          int_box.setFillColor(neighbors > 0 ? sf::Color::Blue : sf::Color::Green) :
          int_box.setFillColor(sf::Color::Red);
      int_box.setOutlineThickness(2 * border_size);
      window.draw(int_box);
      int_box.setOutlineThickness(0);
    }
  }
  if constexpr(std::is_same<T, float>()) {
    gui.set_text(popup(), "");
    sf::CircleShape nearest_circle(update.radius);
    nearest_circle.setOrigin(update.radius, update.radius);
    nearest_circle.setFillColor(sf::Color::Blue);
    nearest_circle.setOutlineThickness(2);
    nearest_circle.setOutlineColor(sf::Color::Red);
    nearest_circle.setPosition(state->selected.x(), state->selected.y());
    window.draw(nearest_circle);

    sf::CircleShape point_shape(4);
    point_shape.setOrigin(4, 4);
    point_shape.setFillColor(sf::Color::Green);
    for(const auto& point : state->points) {
      point_shape.setPosition(point.x(), point.y());
      window.draw(point_shape);
    }
    point_shape.setPosition(state->selected.x(), state->selected.y());
    point_shape.setFillColor(sf::Color::Yellow);
    window.draw(point_shape);

    // TO DO : Draw nearest
  }
}


class K_Near_Gui {
  const sf::Color background = sf::Color(240, 234, 214, 255); // eggshell
  mutable Gui gui;
  Toggle_Button int_button;
  Toggle_Button float_button;

  Text_Display k_value;
  Text_Display popup;
  Push_Button k_decrease;
  Push_Button k_increase;

  Toggle_Button add_mode;
  Toggle_Button del_mode;
  Toggle_Button sel_mode;
  Push_Button undo;
  Push_Button redo;
  Push_Button clear;

  const sf::Color undo_view_color = sf::Color(133, 133, 133);
  std::array<Text_Display, 5> undo_view; 

  Gui int_lhs;
  const int box_size = 50;
  const int border_size = 1;
  std::vector<std::vector<bool>> active_positions;
  sf::RectangleShape int_box;

  Gui float_lhs;

  std::map<size_t, bool> gui_state;
  update_info update_;

  template<typename T>
  void update_mode(sf::RenderWindow&, std::shared_ptr<Mode_State<T>>, size_t&);

public:
  K_Near_Gui(sf::RenderWindow&, const sf::Font&, size_t);
  void update(sf::RenderWindow&, std::map<Mode, std::shared_ptr<Mode_Base>>&, size_t&);
  Mode state() const;
  void draw(sf::RenderWindow&, const std::map<Mode, std::shared_ptr<Mode_Base>>&, size_t) const;
};

K_Near_Gui::K_Near_Gui(sf::RenderWindow& window, const sf::Font& font, size_t neighbors):
  gui(window, font),
  int_button(120, 100, 1030, 50, "INT", true),
  float_button(120, 100, 1150, 50, "FLOAT"), 
  k_value(140, 100, 1080, 250, std::to_string(neighbors),
          Base_Scheme.background_ * sf::Color(200, 200, 200)),
  popup(1300, 24, 0, 1000, ""),
  k_decrease(50, 100, 1030, 250, "<"),
  k_increase(50, 100, 1220, 250, ">"),
  add_mode(70, 70, 1030, 450, "ADD"),
  del_mode(70, 70, 1115, 450, "DEL"),
  sel_mode(70, 70, 1030, 570, "SEL"),
  undo(70, 70, 1200, 450, "UNDO"),
  redo(70, 70, 1200, 570, "REDO"),
  clear(70, 70, 1114, 570, "CLR"),
  undo_view({ Text_Display(240, 50, 1030, 680, "", undo_view_color * sf::Color(0, 0, 0, 150)),
              Text_Display(240, 50, 1030, 760, "", undo_view_color),
              Text_Display(240, 50, 1030, 820, "", undo_view_color),
              Text_Display(240, 50, 1030, 880, "", undo_view_color),
              Text_Display(240, 50, 1030, 940, "", undo_view_color) }),
  int_lhs(window, font),
  active_positions(1000 / box_size, std::vector<bool>(1000 / box_size, false)),
  int_box(sf::Vector2f(box_size - 2 * border_size, box_size - 2 * border_size)),
  float_lhs(window, font)
  {
  //gui.add_background(Background(300, 1000, 1000, 0)); TO DO
  Background back(300, 1000, 1000, 0);
  gui.add_background(back);
  gui.add_linked_toggle(int_button, float_button);
  Text_Display neighbor_label(240, 50, 1030, 200, "Neighbors");
  gui.add_text(neighbor_label, k_value, popup);
  gui.add_push_button(k_decrease, k_increase);
  gui.add_linked_toggle(add_mode, del_mode);
  gui.add_toggle_button(sel_mode);
  gui.add_push_button(undo, redo, clear);
  sf::Color undo_view_color(133, 133, 133);
  Text_Display redo_label(240, 20, 1030, 660, "Redo");
  Text_Display undo_label(240, 20, 1030, 740, "Undo");
  gui.add_text(redo_label, undo_label);
  for(auto& elem : undo_view) { gui.add_text(elem); }
  gui.set_text_color(sf::Color::Black);
  Background int_back(1000, 1000, 0, 0, sf::Color::Black); 
  int_lhs.add_background(int_back);
  for(int i = 1; i < 1000 / box_size; ++i) {
    Text_Display horz_label(box_size - 2 * border_size, box_size - 2 * border_size,
                            i * box_size + border_size, border_size, std::to_string(i));
    Text_Display vert_label(box_size - 2 * border_size, box_size - 2 * border_size, 
                            border_size, i * box_size + border_size, Alphas.substr(i - 1, 1));
    int_lhs.add_text(horz_label, vert_label);
  }
  Text_Display tl_block(box_size - 2 * border_size, box_size - 2 * border_size,
                        border_size, border_size, "");
  int_lhs.add_text(tl_block);
  // TO DO : Only after implimenting GUI lock features
  //std::vector<std::vector<Toggle_Button>> int_positions(1000 / box_size);
  //for(size_t i = 1; i < 1000 / box_size; ++i) {
  //  for(size_t j = 1; j < 1000 / box_size; ++j) {
  //    int_positions[i].emplace_back(box_size - 2 * border_size, box_size - 2 * border_size,
  //                          j * box_size + border_size, i * box_size + border_size, "");
  //    int_lhs.add_toggle_button(*(int_positions[i].end()-1));
  //  }
  //}
  for(int i = 0; i < 1000; i+= 100) {
    Background tick_v(1, 10, i, 0, background); 
    Background tick_h(10, 1, 0, i, background); 
    float_lhs.add_background(tick_v, tick_h);
    Text_Display text_v(20, 16, i, 0, std::to_string(i / 100), sf::Color(0,0,0,0));
    Text_Display text_h(20, 16, 0, i, std::to_string(i / 100), sf::Color(0,0,0,0));
    float_lhs.add_text(text_v, text_h);
  }
  float_lhs.set_text_color(background);
}

template<typename T>
void K_Near_Gui::update_mode(sf::RenderWindow& window, std::shared_ptr<Mode_State<T>> state,
                             size_t& neighbors) {
  if(gui_state[k_increase()]) gui.set_text(k_value(), std::to_string(++neighbors));
  if(gui_state[k_decrease()] && neighbors > 0) gui.set_text(k_value(), std::to_string(--neighbors));

  int position_scale = std::is_same<T, int>() ? 50 : 1;
  if(gui_state[add_mode()] && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if(std::is_same<T, float>()) while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
    add_op<T>(state, sf::Mouse::getPosition(window) / position_scale, active_positions);
  }
  if(gui_state[del_mode()] && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if(std::is_same<T, float>()) while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
    del_op<T>(state, sf::Mouse::getPosition(window) / position_scale, active_positions);
  }
  if(gui_state[sel_mode()] && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    sel_op<T>(state, sf::Mouse::getPosition(window) / position_scale, active_positions);
  }
  if(gui_state[clear()]) clr_op<T>(state, active_positions);
  if(gui_state[undo()]) undo_op<T>(state, active_positions);
  if(gui_state[redo()]) redo_op<T>(state, active_positions);
  update_ = update_state<T>(state, active_positions, neighbors, gui, undo_view);
}

void K_Near_Gui::update(sf::RenderWindow& w,
                        std::map<Mode, std::shared_ptr<Mode_Base>>& state_map,
                        size_t& neighbors) {
  gui_state = gui.get_state();
  Mode mode = state();
  if(mode == int_)
    update_mode<int>(w, std::dynamic_pointer_cast<Mode_State<int>>(state_map[int_]), neighbors);
  else if(mode == float_)
    update_mode<float>(w, std::dynamic_pointer_cast<Mode_State<float>>(state_map[float_]), neighbors);
}

Mode K_Near_Gui::state() const {
  if(gui_state.at(int_button())) return int_;
  else if(gui_state.at(float_button())) return float_;
  else return none_;
}

void K_Near_Gui::draw(sf::RenderWindow& window, 
                      const std::map<Mode, std::shared_ptr<Mode_Base>>& state_map,
                      size_t neighbors) const {
  Mode mode = state();
  if(mode == int_) {
    int_lhs.draw(window);
    draw_state<int>(window, std::dynamic_pointer_cast<Mode_State<int>>(state_map.at(int_)),
                    active_positions, update_, neighbors, gui, popup);
  } else if(mode == float_) {
    draw_state<float>(window, std::dynamic_pointer_cast<Mode_State<float>>(state_map.at(float_)),
                      active_positions, update_, neighbors, gui, popup);
    float_lhs.draw(window);
  }
  gui.draw(window);
}

int main() {

  XInitThreads();

  // window
  const sf::Vector2u window_size(1300, 1024);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "K-Nearest");
  window.setFramerateLimit(60);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }
  // Initializations
  size_t neighbors = 0;
  std::map<Mode, std::shared_ptr<Mode_Base>> state;
  state.emplace(int_, std::make_shared<Mode_State<int>>());
  state.emplace(float_, std::make_shared<Mode_State<float>>());

  K_Near_Gui gui(window, font, neighbors);

  sf::Event event;
  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        default: break;
      }
    } // end event loop

    // UPDATE
    gui.update(window, state, neighbors);

    // DRAW
    window.clear();
    gui.draw(window, state, neighbors);
    window.display();

  } // end game loop

  return 0;
}


// FUTURE REMARKS :
//   allow placing of labels and then an identifier based on k-nearest (can change k), on a tie pick the nearest neighbor,
//     on a tie for nearest neighbor on classification (ie ties in number and then ties in finding nearest (label) then pop up 50 / 50
//   allow weighted option, for which closer elements count as more likely to affect label guess
//
//   No error handing for indexing in k_nearest
