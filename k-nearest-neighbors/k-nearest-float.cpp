#include <iostream> // Debugging headers

#include "Harmony/sfml/gui/gui.h"

#include <math.h>
#include <string>
#include <vector>
#include <array>
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

int main() {

  XInitThreads();

  // window
  const sf::Vector2u window_size(1300, 1024);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "K-Nearest");
  window.setFramerateLimit(60);

  // GUI
  
  sf::Color eggshell = sf::Color(240, 234, 214, 255);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }

  Gui gui(window, font);

//  Background back_1(1000, 1000, 0, 0, sf::Color::Black);
  Background back_2(300, 1000, 1000, 0);
  gui.add_background(back_2);

  // Labels
  std::vector<Background> top_label_tick;
  std::vector<Background> left_label_tick;
  for(int i = 0; i < 1000; i+= 100) {
    top_label_tick.emplace_back(1, 10, i, 0, eggshell); 
    left_label_tick.emplace_back(10, 1, 0, i, eggshell); 
    gui.add_background(top_label_tick[i / 100], left_label_tick[i / 100]);
  }
  std::vector<Text_Display> top_label;
  std::vector<Text_Display> left_label;
  for(int i = 0; i < 1000; i+= 100) {
    top_label.emplace_back(20, 16, i, 0, std::to_string(i / 100), sf::Color(0,0,0,0)); 
    left_label.emplace_back(20, 16, 0, i, std::to_string(i / 100), sf::Color(0,0,0,0)); 
    gui.add_text(top_label[i / 100], left_label[i / 100]);
  }
  gui.set_text_color(eggshell);

  Toggle_Button int_button(120, 100, 1030, 50, "INT");
  Toggle_Button float_button(120, 100, 1150, 50, "FLOAT");
  gui.add_toggle_button(int_button, float_button);
  // TO DO : Link buttons

  // Neighbors GUI Part
  int neighbors = 0;
  Text_Display neighbors_label(240, 50, 1030, 200, "Neighbors");
  Text_Display k_value(140, 100, 1080, 250, std::to_string(neighbors));
  Text_Display popup(1300, 24, 0, 1000, "");
  gui.add_text(neighbors_label, k_value, popup);
  Push_Button k_decrease(50, 100, 1030, 250, "<");
  Push_Button k_increase(50, 100, 1220, 250, ">");
  gui.add_push_button(k_decrease, k_increase);

  Toggle_Button add_mode(70, 70, 1030, 450, "A");
  Toggle_Button del_mode(70, 70, 1115, 450, "D");
  Toggle_Button sel_mode(70, 70, 1030, 570, "S");
  gui.add_toggle_button(add_mode, del_mode, sel_mode);
  Push_Button undo(70, 70, 1200, 450, "U");
  Push_Button redo(70, 70, 1200, 570, "R");
  Push_Button clear(70, 70, 1114, 570, "C");
  gui.add_push_button(undo, redo, clear);

  sf::Color undo_view_color(133, 133, 133);
  std::array<Text_Display, 5> undo_view = { Text_Display(240, 50, 1030, 700, "", 
                                                         undo_view_color * sf::Color(0, 0, 0, 150)),
                                            Text_Display(240, 50, 1030, 760, "", undo_view_color),
                                            Text_Display(240, 50, 1030, 820, "", undo_view_color),
                                            Text_Display(240, 50, 1030, 880, "", undo_view_color),
                                            Text_Display(240, 50, 1030, 940, "", undo_view_color) };
  for(auto& elem : undo_view) { gui.add_text(elem); }

  std::vector<Undoable<float>> undo_vec;
  int undo_pos = -1;

  std::vector<Point<float>> points;
  Point<float> selected(-1, -1);

  sf::CircleShape point_shape(4);
  point_shape.setOrigin(sf::Vector2f(4, 4));

  sf::CircleShape nearest_circle(1);
  nearest_circle.setOrigin(sf::Vector2f(1, 1));
  nearest_circle.setFillColor(sf::Color::Blue);
  nearest_circle.setOutlineThickness(2);
  nearest_circle.setOutlineColor(sf::Color::Red);

  // Initializations
  sf::Event event;
  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::KeyPressed:
          switch(event.key.code) {
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
    if(gui_state[add_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // TO DO : Holding
        while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if(position.x > 0 && position.x < 1000 &&
           position.y > 0 && position.x < 1000) {
          points.emplace_back(position.x, position.y);
          undo_vec.erase(undo_vec.begin() + undo_pos + 1, undo_vec.end());
          undo_vec.emplace_back(added, *(points.end() - 1));
          ++undo_pos;
        }
      }
    }
    if(gui_state[del_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // TO DO : Holding
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if(position.x > 0 && position.x < 1000 &&
           position.y > 0 && position.x < 1000) {
          Point<float> del_pos = Point<float>(position.x, position.y);
          auto idx_nearest = k_nearest(points, del_pos, 1);
          if(!points.empty() && metric(del_pos, points[idx_nearest[0]]) < 8) {
            undo_vec.erase(undo_vec.begin() + undo_pos + 1, undo_vec.end());
            undo_vec.emplace_back(deleted, *(points.begin()+idx_nearest[0]));
            ++undo_pos;
            points.erase(points.begin()+idx_nearest[0], points.begin()+idx_nearest[0]+1);
          } // TO DO : Error when deleting the last guy
        }
      }
    }
    if(gui_state[sel_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // TO DO : holding?
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if(position.x > 0 && position.x < 1000 &&
           position.y > 0 && position.x < 1000) {
          selected = Point<float>(position.x, position.y);
        }
      }
    }
    if(gui_state[clear()]) {
      undo_vec.erase(undo_vec.begin() + undo_pos + 1, undo_vec.end());
      undo_vec.emplace_back(cleared, points);
      ++undo_pos;
      points.clear();
    }
    if(gui_state[undo()]) {
      if(undo_pos > -1) {
        switch(undo_vec[undo_pos].what) {
          case added:
            points.erase(points.end() - 1, points.end());
            break;
          case deleted:
            points.push_back(undo_vec[undo_pos].points[0]);
            break;
          case cleared:
            points = undo_vec[undo_pos].points;
            break;
        }
        --undo_pos;
      }
    }
    if(gui_state[redo()]) {
      if(undo_pos + 1 < undo_vec.size()) {
        switch(undo_vec[undo_pos + 1].what) {
          case added:
            points.push_back(undo_vec[undo_pos + 1].points[0]);
            break;
          case deleted:
            points.erase(points.end() - 1, points.end());
            break;
          case cleared:
            points.clear();
            break;
        }
        ++undo_pos;
      }
    }

    // UPDATE

    // Get k nearest points indicies
    auto k_near = k_nearest(points, selected, neighbors);
    // Calculate radius for k nearest points
    float rad = 1;
    for(const auto& idx : k_near) {
      float test_rad = metric(points[idx], selected);
      if(test_rad > rad) rad = test_rad;
    }
    nearest_circle.setPosition(0, 0);
    nearest_circle.setRadius(rad);
    nearest_circle.setOrigin(rad, rad);
    if(undo_pos + 1 < undo_vec.size()) 
      gui.set_text(undo_view[0](), undoable_to_string(undo_vec[undo_pos + 1]));
    else  gui.set_text(undo_view[0](), "");
    for(int i = 0; i < 4; ++i) {
      gui.set_text(undo_view[i + 1](), 
                   i <= undo_pos ? undoable_to_string(undo_vec[undo_pos - i]) : "");
    }

    //DRAW
    window.clear(sf::Color::Black);


    nearest_circle.setPosition(selected.x(), selected.y());
    window.draw(nearest_circle);

    for(const auto& point : points) { 
      point_shape.setPosition(point.x(), point.y());
      window.draw(point_shape);
    }
    point_shape.setPosition(selected.x(), selected.y());
    point_shape.setFillColor(sf::Color::Yellow);
    window.draw(point_shape);
    point_shape.setFillColor(sf::Color::Green);

    // Draw Grid
    // Draw k nearest points
    // Draw Selected Element ( fixes border issues ) // TO DO : Stop circle before moved
    
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
