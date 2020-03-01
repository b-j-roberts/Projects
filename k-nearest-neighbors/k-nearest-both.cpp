#include <iostream> // Debugging headers

#include "Harmony/sfml/gui/gui.h"

#include <math.h>
#include <string>
#include <vector>
#include <array>
#include <memory> // shared_ptr
#include <algorithm>
#include <stdexcept>

#include <sstream>
#include <iomanip>

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

/*****************************
 *
 *         METRICS           
 *
 *****************************/

int min(int a, int b) { return a < b ? a : b; }

// Metrics return a const value of the distance between the two point objects given
template<typename T>
const T metric(const Point<T>& p1, const Point<T>& p2);

// NO BARRIER METRICS

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


/******************************
 *
 *    k-nearest functionality
 *
 ******************************/

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
  for(size_t i = 0, end = points.size(); i < end; ++i) {
    pos.emplace_back(dist[i], i);
  }
  //std::transform(dist.begin(), dist.end(), std::back_inserter(pos), [i = 0](const auto& elem){
  //    return pos_holder<T>(*elem, i++); };

  // TO DO : Partial sort?
  std::sort(pos.begin(), pos.end(), [&](const pos_holder<T>& a,const pos_holder<T>& b) -> bool {
    return a.value < b.value;
  });

  // Take k closest values and return the index of them
  std::vector<int> ret;
  for(size_t i = 0, end = min(k, points.size()); i < end; ++i) ret.push_back(pos[i].pos);
  return ret;
}

// Same as other k_nearest (see above) but for a point inside of the vector at position idx
template<typename T>
const std::vector<int> k_nearest(const std::vector<Point<T>>& points, size_t idx, size_t k) {
   auto ret = k_nearest(points, points[idx], k + 1);
   ret.erase(std::remove(ret.begin(), ret.end(), idx), ret.end());
   return ret;
}



enum undo_type { added, deleted, cleared };

template<typename T>
struct Undoable {
  undo_type what;
  std::vector<Point<T>> points;
  Undoable(undo_type w, Point<T> p): what(w), points(1, p) { }
  Undoable(undo_type w, std::vector<Point<T>> vec_p): what(w), points(vec_p) { }
};

const std::string Alphas = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string undoable_to_string(const Undoable<int>& u) {
  switch(u.what) {
    case added:
      return "Added " + Alphas.substr(u.points[0].y()-1, 1) + ":" + std::to_string(u.points[0].x());
      break;
    case deleted:
      return "Deleted " + Alphas.substr(u.points[0].y()-1, 1) + ":" + std::to_string(u.points[0].x());
      break;
    case cleared:
      return "Cleared!";
      break;
  }
  return "";
}
std::string undoable_to_string(const Undoable<float>& u) {
  std::stringstream out;
  switch(u.what) {
    case added:
      out << "Added (" << std::fixed << std::setprecision(2) << u.points[0].y() / 100.f
          << ", " << u.points[0].x() / 100.f << ")";
      break;
    case deleted:
      out << "Deleted (" << std::fixed << std::setprecision(2) << u.points[0].y() / 100.f
          << ", " << u.points[0].x() / 100.f << ")";
      break;
    case cleared:
      return "Cleared!";
      break;
  }
  return out.str();
}

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

enum Mode { int_ = 0, float_ };

void add_op(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           const sf::Vector2i& position, std::vector<std::vector<bool>>& active_positions) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
    auto int_pos = position / 50; // TO DO :
    if(int_pos.x < active_positions[0].size() && int_pos.x > 0 &&
       int_pos.y < active_positions.size() && int_pos.y > 0) {
      if(!active_positions[int_pos.y][int_pos.x]) {
        active_positions[int_pos.y][int_pos.x] = true;
        s->points.emplace_back(int_pos.x, int_pos.y);
            s->undo_vec.erase(s->undo_vec.begin() + s->undo_pos + 1, s->undo_vec.end());
                    s->undo_vec.emplace_back(added, *(s->points.end() - 1));
        ++(s->undo_pos);
      }
    }
  } else {
    while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    if(position.x > 0 && position.x < 1000 &&
       position.y > 0 && position.x < 1000) {
      s->points.emplace_back(position.x, position.y);
      s->undo_vec.erase(s->undo_vec.begin() + s->undo_pos + 1, s->undo_vec.end());
      s->undo_vec.emplace_back(added, *(s->points.end() - 1));
      ++(s->undo_pos);
    }
  }
}

void del_op(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           const sf::Vector2i& position, std::vector<std::vector<bool>>& active_positions) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
    auto int_pos = position / 50; // TO DO :
    if(int_pos.x < active_positions[0].size() && int_pos.x > 0 &&
       int_pos.y < active_positions.size() && int_pos.y > 0) {
      if(active_positions[int_pos.y][int_pos.x]) {
      s->undo_vec.erase(s->undo_vec.begin() + s->undo_pos + 1, s->undo_vec.end());
                s->undo_vec.emplace_back(deleted, Point<int>(int_pos.x, int_pos.y));
        ++(s->undo_pos);
        active_positions[int_pos.y][int_pos.x] = false;
        s->points.erase(std::remove(s->points.begin(), s->points.end(),
                                 Point<int>(int_pos.x, int_pos.y)), s->points.end());
      }
    }
  } else {
    while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    if(position.x > 0 && position.x < 1000 &&
       position.y > 0 && position.x < 1000) {
      Point<float> del_pos = Point<float>(position.x, position.y);
      auto idx_nearest = k_nearest(s->points, del_pos, 1);
      if(!s->points.empty() && metric(del_pos, s->points[idx_nearest[0]]) < 8) {
        s->undo_vec.erase(s->undo_vec.begin() + s->undo_pos + 1, s->undo_vec.end());
        s->undo_vec.emplace_back(deleted, *(s->points.begin()+idx_nearest[0]));
        ++s->undo_pos;
        s->points.erase(s->points.begin()+idx_nearest[0], s->points.begin()+idx_nearest[0]+1);
      } // TO DO : Error when deleting the last guy
    }
  }
}

void sel_op(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           const sf::Vector2i& position, std::vector<std::vector<bool>>& active_positions) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
    auto int_pos = position / 50; // TO DO :
    if(int_pos.x < active_positions[0].size() && int_pos.x > 0 &&
      int_pos.y < active_positions.size() && int_pos.y > 0) {
      s->selected = Point<int>(int_pos.x, int_pos.y);
    }
  } else {
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    if(position.x > 0 && position.x < 1000 &&
       position.y > 0 && position.x < 1000) {
      s->selected = Point<float>(position.x, position.y);
    }
  }
}

void clr_op(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           std::vector<std::vector<bool>>& active_positions) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
    s->undo_vec.erase(s->undo_vec.begin() + s->undo_pos + 1, s->undo_vec.end());
                s->undo_vec.emplace_back(cleared, s->points);
    ++(s->undo_pos);
    s->points.clear();
    for(auto& rows : active_positions) {
      std::fill(rows.begin(), rows.end(), false);
    }
  } else {
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    s->undo_vec.erase(s->undo_vec.begin() + s->undo_pos + 1, s->undo_vec.end());
    s->undo_vec.emplace_back(cleared, s->points);
    ++(s->undo_pos);
    s->points.clear();
  }
}

void undo_op(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           std::vector<std::vector<bool>>& active_positions) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
    if(s->undo_pos > -1) {
      switch(s->undo_vec[s->undo_pos].what) {
        case added:
          active_positions[s->undo_vec[s->undo_pos].points[0].y()]
                          [s->undo_vec[s->undo_pos].points[0].x()] = false;
                              s->points.erase(s->points.end() - 1, s->points.end());
          break;
        case deleted:
          active_positions[s->undo_vec[s->undo_pos].points[0].y()]
                          [s->undo_vec[s->undo_pos].points[0].x()] = true;
                              s->points.push_back(s->undo_vec[s->undo_pos].points[0]);
          break;
        case cleared:
        s->points = s->undo_vec[s->undo_pos].points;
          for(const auto& point : s->points) active_positions[point.y()][point.x()] = true;
          break;
      }
      --(s->undo_pos);
    }
  } else {
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    if(s->undo_pos > -1) {
      switch(s->undo_vec[s->undo_pos].what) {
        case added:
          s->points.erase(s->points.end() - 1, s->points.end());
          break;
        case deleted:
          s->points.push_back(s->undo_vec[s->undo_pos].points[0]);
          break;
        case cleared:
          s->points = s->undo_vec[s->undo_pos].points;
          break;
      }
      --(s->undo_pos);
    }
  }
}

void redo_op(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           std::vector<std::vector<bool>>& active_positions) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
    if(s->undo_pos + 1 < s->undo_vec.size()) {
      switch(s->undo_vec[s->undo_pos + 1].what) {
        case added:
          active_positions[s->undo_vec[s->undo_pos + 1].points[0].y()]
                          [s->undo_vec[s->undo_pos + 1].points[0].x()] = true;
                              s->points.push_back(s->undo_vec[s->undo_pos + 1].points[0]);
          break;
        case deleted:
          active_positions[s->undo_vec[s->undo_pos + 1].points[0].y()]
                          [s->undo_vec[s->undo_pos + 1].points[0].x()] = false;
                              s->points.erase(s->points.end() - 1, s->points.end());
          break;
        case cleared:
          s->points.clear();
          for(auto& rows : active_positions) {
            std::fill(rows.begin(), rows.end(), false);
          }
          break;
      }
      ++(s->undo_pos);
    }
  } else {
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    if(s->undo_pos + 1 < s->undo_vec.size()) {
      switch(s->undo_vec[s->undo_pos + 1].what) {
        case added:
          s->points.push_back(s->undo_vec[s->undo_pos + 1].points[0]);
          break;
        case deleted:
          s->points.erase(s->points.end() - 1, s->points.end());
          break;
        case cleared:
          s->points.clear();
          break;
      }
      ++(s->undo_pos);
    }
  }
}

struct update_info {
  std::vector<int> k_near;
  float radius;
};

update_info update_state(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode,
           std::vector<std::vector<bool>>& active_positions,
           Gui& gui, const std::array<Text_Display, 5>& undo_view, size_t neighbors) {

  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
   // Get k nearest points indicies
    update_info ret{k_nearest(s->points, s->selected, neighbors)};
    // Calculate radius for k nearest points
    int rad = 0;
    for(auto val : ret.k_near) {
      auto dist = metric(s->points[val], s->selected);
      if(dist > rad) rad = dist;
    }
    ret.radius = rad;
    if(s->undo_pos + 1 < s->undo_vec.size())
      gui.set_text(undo_view[0](), undoable_to_string(s->undo_vec[s->undo_pos + 1]));
    else  gui.set_text(undo_view[0](), "");
    for(int i = 0; i < 4; ++i) {
      gui.set_text(undo_view[i + 1](),
                   i <= s->undo_pos ? undoable_to_string(s->undo_vec[s->undo_pos - i]) : "");
    }
    return ret;
  } else {
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    update_info ret{k_nearest(s->points, s->selected, neighbors)};
    // Calculate radius for k nearest points
    float rad = 1;
    for(const auto& idx : ret.k_near) {
      float test_rad = metric(s->points[idx], s->selected);
      if(test_rad > rad) rad = test_rad;
    }
    ret.radius = rad;
    if(s->undo_pos + 1 < s->undo_vec.size())
      gui.set_text(undo_view[0](), undoable_to_string(s->undo_vec[s->undo_pos + 1]));
    else  gui.set_text(undo_view[0](), "");
    for(int i = 0; i < 4; ++i) {
      gui.set_text(undo_view[i + 1](),
                   i <= s->undo_pos ? undoable_to_string(s->undo_vec[s->undo_pos - i]) : "");
    }
    return ret;
  }

}

void draw_state(std::map<Mode, std::shared_ptr<Mode_Base>>& state, Mode mode, 
                std::vector<std::vector<bool>>& active_positions, sf::RenderWindow& window,
                const update_info& update, Gui& gui, const Text_Display& popup, size_t neighbors) {
  if(mode == int_) {
    auto s = std::dynamic_pointer_cast<Mode_State<int>>(state[int_]);
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
        int dist = metric(Point(j, i), s->selected);
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
      int_box.setPosition(s->points[val].x() * box_size + border_size,
                          s->points[val].y() * box_size + border_size);
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
    if(s->selected.x() > 0 && s->selected.x() < active_positions[0].size() &&
       s->selected.y() > 0 && s->selected.y() < active_positions.size()) {
      int_box.setPosition(s->selected.x() * box_size + border_size,
                          s->selected.y() * box_size + border_size);
      active_positions[s->selected.y()][s->selected.x()] ?
          int_box.setFillColor(neighbors > 0 ? sf::Color::Blue : sf::Color::Green) :
          int_box.setFillColor(sf::Color::Red);
      int_box.setOutlineThickness(2 * border_size);
      window.draw(int_box);
      int_box.setOutlineThickness(0);
    }
  } else {
    gui.set_text(popup(), "");
    auto s = std::dynamic_pointer_cast<Mode_State<float>>(state[float_]);
    sf::CircleShape nearest_circle(update.radius);
    nearest_circle.setPosition(0, 0);
    nearest_circle.setOrigin(update.radius, update.radius);
    nearest_circle.setFillColor(sf::Color::Blue);
    nearest_circle.setOutlineThickness(2);
    nearest_circle.setOutlineColor(sf::Color::Red);
    nearest_circle.setPosition(s->selected.x(), s->selected.y());
    window.draw(nearest_circle);

    sf::CircleShape point_shape(4);
    point_shape.setOrigin(sf::Vector2f(4, 4));
    point_shape.setFillColor(sf::Color::Green);
    for(const auto& point : s->points) {
      point_shape.setPosition(point.x(), point.y());
      window.draw(point_shape);
    }
    point_shape.setPosition(s->selected.x(), s->selected.y());
    point_shape.setFillColor(sf::Color::Yellow);
    window.draw(point_shape);

    // TO DO : Draw nearest
  }
}
int main() {

  XInitThreads();

  // window
  const sf::Vector2u window_size(1300, 1024);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "K-Nearest");
  window.setFramerateLimit(60);

  // RHS GUI
  
  sf::Color eggshell = sf::Color(240, 234, 214, 255);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }

  Gui gui(window, font);

  Background back(300, 1000, 1000, 0);
  gui.add_background(back);

  Toggle_Button int_button(120, 100, 1030, 50, "INT", true);
  Toggle_Button float_button(120, 100, 1150, 50, "FLOAT");
  gui.add_linked_toggle(int_button, float_button);

  // Neighbors GUI Part
  int neighbors = 0;
  Text_Display neighbors_label(240, 50, 1030, 200, "Neighbors");
  Text_Display k_value(140, 100, 1080, 250, std::to_string(neighbors), 
                       Base_Scheme.background_ * sf::Color(200, 200, 200));
  Text_Display popup(1300, 24, 0, 1000, "");
  gui.add_text(neighbors_label, k_value, popup);
  Push_Button k_decrease(50, 100, 1030, 250, "<");
  Push_Button k_increase(50, 100, 1220, 250, ">");
  gui.add_push_button(k_decrease, k_increase);

  // Modes and buttons
  Toggle_Button add_mode(70, 70, 1030, 450, "ADD");
  Toggle_Button del_mode(70, 70, 1115, 450, "DEL");
  Toggle_Button sel_mode(70, 70, 1030, 570, "SEL");
  gui.add_linked_toggle(add_mode, del_mode);
  gui.add_toggle_button(sel_mode);
  Push_Button undo(70, 70, 1200, 450, "UNDO");
  Push_Button redo(70, 70, 1200, 570, "REDO");
  Push_Button clear(70, 70, 1114, 570, "CLR");
  gui.add_push_button(undo, redo, clear);

  // Undo / Redo
  sf::Color undo_view_color(133, 133, 133);
  Text_Display redo_name(240, 20, 1030, 660, "Redo");
  Text_Display undo_name(240, 20, 1030, 740, "Undo");
  gui.add_text(redo_name, undo_name);
  std::array<Text_Display, 5> undo_view = { Text_Display(240, 50, 1030, 680, "", 
                                                         undo_view_color * sf::Color(0, 0, 0, 150)),
                                            Text_Display(240, 50, 1030, 760, "", undo_view_color),
                                            Text_Display(240, 50, 1030, 820, "", undo_view_color),
                                            Text_Display(240, 50, 1030, 880, "", undo_view_color),
                                            Text_Display(240, 50, 1030, 940, "", undo_view_color) };
  for(auto& elem : undo_view) { gui.add_text(elem); }
  gui.set_text_color(sf::Color::Black);


  // LHS GUI

  Gui int_lhs(window, font);
  Gui float_lhs(window, font);

  // int
  const int box_size = 50;
  const int border_size = 1;
  Background back_black(1000, 1000, 0, 0, sf::Color::Black);
  int_lhs.add_background(back_black);
  // selection regions
  /* TO DO : Only after implimenting GUI lock features
  std::vector<std::vector<Toggle_Button>> int_positions(1000 / box_size);
  for(size_t i = 1; i < 1000 / box_size; ++i) {
    for(size_t j = 1; j < 1000 / box_size; ++j) {
      int_positions[i].emplace_back(box_size - 2 * border_size, box_size - 2 * border_size,
                            j * box_size + border_size, i * box_size + border_size, "");
      int_lhs.add_toggle_button(*(int_positions[i].end()-1));
    }
  }*/
  // int labels
  std::vector<Text_Display> horz_label;
  std::vector<Text_Display> vert_label;
  for(int i = 1; i < 1000 / box_size; ++i) {
    horz_label.emplace_back(box_size - 2 * border_size, box_size - 2 * border_size, 
                            i * box_size + border_size, border_size, std::to_string(i));
    vert_label.emplace_back(box_size - 2 * border_size, box_size - 2 * border_size, 
                            border_size, i * box_size + border_size, Alphas.substr(i - 1, 1));
    int_lhs.add_text(horz_label[i - 1]);
    int_lhs.add_text(vert_label[i - 1]);
  }
  Text_Display tl_block(box_size - 2 * border_size, box_size - 2 * border_size, 
                        border_size, border_size, "");
  int_lhs.add_text(tl_block);

  // Left Side Interface ( Non - int_lhs )
  // [y][x]
  std::vector<std::vector<bool>> active_positions(1000 / box_size, 
                                                  std::vector<bool>(1000 / box_size, false));
  sf::RectangleShape int_box(sf::Vector2f(box_size - 2 * border_size, box_size - 2 * border_size));
  int_box.setOutlineColor(sf::Color::Yellow);

  // float
  // float Labels
  std::vector<Background> top_label_tick;
  std::vector<Background> left_label_tick;
  for(int i = 0; i < 1000; i+= 100) {
    top_label_tick.emplace_back(1, 10, i, 0, eggshell);
    left_label_tick.emplace_back(10, 1, 0, i, eggshell);
    float_lhs.add_background(top_label_tick[i / 100], left_label_tick[i / 100]);
  }
  std::vector<Text_Display> top_label;
  std::vector<Text_Display> left_label;
  for(int i = 0; i < 1000; i+= 100) {
    top_label.emplace_back(20, 16, i, 0, std::to_string(i / 100), sf::Color(0,0,0,0));
    left_label.emplace_back(20, 16, 0, i, std::to_string(i / 100), sf::Color(0,0,0,0));
    float_lhs.add_text(top_label[i / 100], left_label[i / 100]);
  }
  float_lhs.set_text_color(eggshell);

  // Initializations

  // TO DO : Mode checking for at least one mode
  Mode mode = int_;
  std::map<Mode, std::shared_ptr<Mode_Base>> state;
  state.emplace(int_, std::make_shared<Mode_State<int>>());
  state.emplace(float_, std::make_shared<Mode_State<float>>());

  sf::Event event;
  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::KeyPressed:
          switch(event.key.code) {
            case sf::Keyboard::Down:
              /*if(state.s(mode)->selected.y() + 1 < active_positions.size()) 
                state.s(mode).selected.move(0, 1);
              break;
            case sf::Keyboard::Up:
              if(state.s(mode).selected.y() - 1 >= 0) state.s(mode).selected.move(0, -1);
              break;
            case sf::Keyboard::Right:
              if(state.s(mode).selected.x() + 1 < active_positions[0].size()) state.s(mode).selected.move(1, 0);
              break;
            case sf::Keyboard::Left:
              if(state.s(mode).selected.x() - 1 >= 0) state.s(mode).selected.move(-1, 0);
              break;*/
            case sf::Keyboard::N:
              gui.set_text(k_value(), std::to_string(++neighbors));
              break;
            case sf::Keyboard::B:
              if(neighbors > 0) gui.set_text(k_value(), std::to_string(--neighbors));
              break;
            default: break;
          }
          break;
        case sf::Event::Closed:
          window.close();
          break;

        default: break;
      }
    } // end event loop

    auto gui_state = gui.get_state();
    if(gui_state[k_increase()]) {
      gui.set_text(k_value(), std::to_string(++neighbors));
    }
    if(gui_state[k_decrease()]) {
      if(neighbors > 0) gui.set_text(k_value(), std::to_string(--neighbors));
    }

    if(gui_state[int_button()]) { mode = int_; }
    if(gui_state[float_button()]) { mode = float_; }
    if(gui_state[add_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        add_op(state, mode, position, active_positions);
      }
    }
    if(gui_state[del_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        del_op(state, mode, position, active_positions);
      }
    }
    if(gui_state[sel_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        sel_op(state, mode, position, active_positions);
      }
    }
    if(gui_state[clear()]) {
      clr_op(state, mode, active_positions);
    }
    if(gui_state[undo()]) {
      undo_op(state, mode, active_positions);
    }
    if(gui_state[redo()]) {
      redo_op(state, mode, active_positions);
    }

    // UPDATE

    auto update = update_state(state, mode, active_positions, gui, undo_view, neighbors);

    //DRAW
    window.clear();

    if(mode == int_) int_lhs.draw(window);

    draw_state(state, mode, active_positions, window, update, gui, popup, neighbors);
    
    if(mode == float_) float_lhs.draw(window);
    gui.draw(window);

    window.display();

  } // end game loop

  /*
  bool label_mode = false;
  float radius;

  // Ability to toggle float mode, Resets
  // Ability to toggle label mode, Resets

  if(float_mode) {
    std::vector<Point<float>> float_points;
    int index; // Set to most recent selection (or just keep it there till changed)
   
    // Ability to set neighbors, not reset

    if(!label_mode) {
      
      std::vector<int> nearest_pos = k_nearest(float_points, index, neighbors);
      // color nearest points
      
      radius = metric(float_points[index], float_points[nearest_pos[nearest_pos.size() - 1]]);

    } else {
      
      Point<float> unlabeled; // Placed unlabeled point to find the k nearest to for label mode

      std::vector<int> nearest_pos = k_nearest(float_points, unlabeled, neighbors);

      // set unlabeled label based on predictor function (include weighted and unweighted option, togglable)

      radius = metric(float_points[index], float_points[nearest_pos[nearest_pos.size() - 1]]);

    }
    // draw points (if in label mode draw labels too)
    // draw circle
    // reset colors

  } else {
    std::vector<Point<int>> int_points;
    
    // Ability to set neighbors
  
    // grid view

  }
  */

  return 0;
}


// FUTURE REMARKS :
//   allow placing of labels and then an identifier based on k-nearest (can change k), on a tie pick the nearest neighbor,
//     on a tie for nearest neighbor on classification (ie ties in number and then ties in finding nearest (label) then pop up 50 / 50
//   allow weighted option, for which closer elements count as more likely to affect label guess
//
//   No error handing for indexing in k_nearest
