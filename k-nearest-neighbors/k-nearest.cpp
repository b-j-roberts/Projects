#include <iostream> // Debugging headers

#include "Harmony/SFML/GUI/gui.h"

#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

template<typename T, typename L = std::string>
class Point {

  T x_;
  T y_;

  const L label_;

public:

  Point(T x, T y): x_(x), y_(y) { }
  Point(T x, T y, L label): x_(x), y_(y), label_(label) { }

  Point(const Point& p) = default;
  Point& operator= (const Point& p) = default;

  const T x() const { return x_; }
  const T y() const { return y_; } 

  const L& label() const { return label_; }

};


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


int main() {

  XInitThreads();

  // window
  const sf::Vector2u window_size(1300, 1000);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "K-Nearest");
  window.setFramerateLimit(60);

  // GUI
  
  sf::Color eggshell = sf::Color(240, 234, 214, 255);
  sf::Color gold = sf::Color(212, 175, 55);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }

  GUI gui(window, font);

  Background back_1(1000, 1000, 0, 0, eggshell);
  Background back_2(300, 1000, 1000, 0, gold);
  gui.add_background(back_1, back_2);

  Toggle_Button int_button(120, 100, 1030, 50, "INT");
  Toggle_Button float_button(120, 100, 1150, 50, "FLOAT");
  gui.add_toggle_button(int_button, float_button);
  // TO DO : Link buttons

  // Neighbors GUI Part
  int neighbors = 0;
  Text_Display neighbors_label(240, 50, 1030, 200, "Neighbors", gold);
  Text_Display k_value(140, 100, 1080, 250, std::to_string(neighbors));
  gui.add_text(neighbors_label, k_value);
  Push_Button k_decrease(50, 100, 1030, 250, "<");
  Push_Button k_increase(50, 100, 1220, 250, ">");
  gui.add_push_button(k_decrease, k_increase);

  Toggle_Button add_mode(70, 70, 1030, 550, "A");
  Toggle_Button del_mode(70, 70, 1115, 550, "D");
  Toggle_Button sel_mode(70, 70, 1030, 670, "S");
  gui.add_toggle_button(add_mode, del_mode, sel_mode);
  Push_Button undo(70, 70, 1200, 550, "U");
  gui.add_push_button(undo);

  // Left Side Interface
  size_t box_size = 50;
  size_t border_size = 1;
  // [y][x]
  std::vector<std::vector<bool>> active_positions(1000 / box_size, 
                                                  std::vector<bool>(1000 / box_size, false));
  sf::RectangleShape int_box(sf::Vector2f(box_size - 2 * border_size, box_size - 2 * border_size));
  int_box.setOutlineThickness(border_size);
  int_box.setOutlineColor(sf::Color::Black);

  sf::Vector2i selected(0, 0);

  // Initializations
  sf::Event event;
  auto gui_state = gui.get_state();
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
    gui_state = gui.get_state();
    if(gui_state[k_increase()]) {
      gui.set_text(k_value(), std::to_string(++neighbors));
    }
    if(gui_state[k_decrease()]) {
      gui.set_text(k_value(), std::to_string(--neighbors));
    }
    if(gui_state[add_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if(position.x < 1000 && position.y < 1000) {
          if(!active_positions[position.y / box_size][position.x / box_size]) {
            active_positions[position.y / box_size][position.x / box_size] = true;
          }
        }
      }
    }
    if(gui_state[del_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if(position.x < 1000 && position.y < 1000) {
          if(active_positions[position.y / box_size][position.x / box_size]) {
            active_positions[position.y / box_size][position.x / box_size] = false;
          }
        }
      }
    }
    if(gui_state[sel_mode()]) {
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if(position.x < 1000 && position.y < 1000) {
          selected = sf::Vector2i(position.x / box_size, position.y / box_size);
        }
      }
      /*if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        selected += sf::Vector2i(0, 1);
      }*/
    }

    //DRAW
    window.clear();

    gui.draw(window);

    // Draw Grid
    for(int i = 0; i < active_positions.size(); ++i) {
      for(int j = 0; j < active_positions[i].size(); ++j) {
        int_box.setPosition(j * box_size + border_size, i * box_size + border_size);
        active_positions[i][j] ? int_box.setFillColor(sf::Color::Green) :
                                 int_box.setFillColor(sf::Color::Red);
        if(metric(Point(j, i), Point(selected.x, selected.y)) == 5) 
          int_box.setFillColor(sf::Color::Yellow);
        window.draw(int_box);
      }
    }
    // Draw Selected Element ( fixes border issues )
    int_box.setPosition(selected.x * box_size + border_size, selected.y * box_size + border_size);
    active_positions[selected.y][selected.x] ? int_box.setFillColor(sf::Color::Green) :
                                               int_box.setFillColor(sf::Color::Red);
    int_box.setOutlineThickness(2 * border_size);
    int_box.setOutlineColor(sf::Color::Yellow);
    window.draw(int_box);
    int_box.setOutlineThickness(border_size);
    int_box.setOutlineColor(sf::Color::Black);
    
    
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
//   Include a circle around K-nearest in the visual
//   allow placing of labels and then an identifier based on k-nearest (can change k), on a tie pick the nearest neighbor,
//     on a tie for nearest neighbors include all ties too (highlight and make it pop up with a note) 
//     on a tie for nearest neighbor on classification (ie ties in number and then ties in finding nearest (label) then pop up 50 / 50
//   allow weighted option, for which closer elements count as more likely to affect label guess
//
//   No error handing for indexing in k_nearest
