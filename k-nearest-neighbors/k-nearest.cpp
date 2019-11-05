#include <iostream> // Debugging headers

#include <SFML/Graphics.hpp>

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


/*******************************
 *
 *
 *     GUI FUNCTIONALITY
 *
 *
 ******************************/

class GUI_Toggle_Button {

  sf::RectangleShape button_;
  sf::Text label_;

  bool active_;

public:

  void activate() {
    active_ = true;
    button_.setFillColor(sf::Color(0, 170, 0, 255));
  }

  void deactivate() {
    active_ = false;
    button_.setFillColor(sf::Color(170, 0, 0, 255));
  }

  bool is_active() { return active_; }

  bool in_bounds(const sf::Vector2i& pos) {
    sf::Vector2f position = button_.getPosition();
    sf::Vector2f size = button_.getSize();
    if(pos.x > position.x && pos.x < position.x + size.x &&
       pos.y > position.y && pos.y < position.y + size.y) return true;
    return false;
  }

  GUI_Toggle_Button(const sf::Vector2f& position, const sf::Vector2f& size, 
                    const sf::Font& font, const std::string& label, bool act = false): 
    active_(act) { 
    
    button_.setSize(size);
    button_.setPosition(position);
    if(act) button_.setFillColor(sf::Color(0, 170, 0, 255));
    else    button_.setFillColor(sf::Color(170, 0, 0, 255));
    button_.setOutlineColor(sf::Color(100, 100, 0, 255));
    button_.setOutlineThickness(3);

    label_.setFont(font);
    label_.setString(label);
    label_.setCharacterSize(size.x / label.length() * 1.5); // TO DO : max between height and side
    label_.setFillColor(sf::Color::Black);
    float x_orig = label_.getGlobalBounds().width / 2.f; 
    float y_orig = label_.getGlobalBounds().height;
    label_.setOrigin(x_orig, y_orig);
    label_.setPosition(position.x + size.x / 2, position.y + size.y / 3); // TO DO: Figure this out
  }

  void draw(sf::RenderWindow& window) {
    window.draw(button_);
    window.draw(label_);
  }

};

class GUI_Click_Button {

  sf::RectangleShape button_;
  sf::Text label_;

public:

  bool in_bounds(const sf::Vector2i& pos) {
    sf::Vector2f position = button_.getPosition();
    sf::Vector2f size = button_.getSize();
    if(pos.x > position.x && pos.x < position.x + size.x &&
       pos.y > position.y && pos.y < position.y + size.y) return true;
    return false;
  }

  // TO DO : Slight color change to indicate click? 
  
  GUI_Click_Button(const sf::Vector2f& position, const sf::Vector2f& size,
                   const sf::Font& font, const std::string& label) {

    button_.setSize(size);
    button_.setPosition(position);
    button_.setFillColor(sf::Color(170, 0, 0, 255));
    button_.setOutlineColor(sf::Color(100, 100, 0, 255));
    button_.setOutlineThickness(3);

    label_.setFont(font);
    label_.setString(label);
    label_.setCharacterSize(size.x / label.length() * 1.5); // TO DO : max between height and side
    label_.setFillColor(sf::Color::Black);
    float x_orig = label_.getGlobalBounds().width / 2.f;
    float y_orig = label_.getGlobalBounds().height;
    label_.setOrigin(x_orig, y_orig);
    label_.setPosition(position.x + size.x / 2, position.y + size.y / 3); // TO DO: Figure this out
  }

  void draw(sf::RenderWindow& window) {
    window.draw(button_);
    window.draw(label_);
  }

};


int main() {

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

  sf::RectangleShape left_side_background(sf::Vector2f(994, 994));
  left_side_background.setFillColor(eggshell);
  left_side_background.setOutlineColor(sf::Color(100, 100, 0, 255));
  left_side_background.setOutlineThickness(3);
  left_side_background.setPosition(sf::Vector2f(3, 3));

  sf::RectangleShape right_side_background(sf::Vector2f(294, 994));
  right_side_background.setFillColor(gold);
  right_side_background.setOutlineColor(sf::Color(100, 100, 0, 255));
  right_side_background.setOutlineThickness(3);
  right_side_background.setPosition(sf::Vector2f(1003, 3));

  GUI_Toggle_Button int_mode_button(sf::Vector2f(1030, 50), sf::Vector2f(120, 100), font, "INT",  true);
  GUI_Toggle_Button float_mode_button(sf::Vector2f(1150, 50), sf::Vector2f(120, 100), font, "FLOAT");

  sf::RectangleShape k_background(sf::Vector2f(240, 100));
  k_background.setFillColor(eggshell);
  k_background.setOutlineColor(sf::Color(100, 100, 0, 255));
  k_background.setOutlineThickness(3);
  k_background.setPosition(sf::Vector2f(1030, 200));

  GUI_Click_Button k_dec_button(sf::Vector2f(1030, 200), sf::Vector2f(50, 100), font, "D");
  GUI_Click_Button k_inc_button(sf::Vector2f(1220, 200), sf::Vector2f(50, 100), font, "I");

  GUI_Toggle_Button add_mode_button(sf::Vector2f(1030, 350), sf::Vector2f(70, 70), font, "A"); 
  GUI_Toggle_Button del_mode_button(sf::Vector2f(1115, 350), sf::Vector2f(70, 70), font, "D"); 
  GUI_Click_Button undo_button(sf::Vector2f(1200, 350), sf::Vector2f(70, 70), font, "U"); 

  // TO DO :
  //         current selected coordinates


  // Initializations
  bool float_mode = false;
  bool del_mode = false;
  bool undo = false;
  sf::Vector2i mouse_pos;

  sf::Event event;

  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
      /*
        case sf::Event::KeyPressed:
          switch(event.key.code) {
            case sf::Keyboard::W:
              break;
            // ...
            default: break;
          }
          break; // key pressed
       */
          case sf::Event::Closed:
            window.close();
            break;

          case sf::Event::MouseButtonPressed:
            if(event.mouseButton.button == sf::Mouse::Left) {
              mouse_pos = sf::Mouse::getPosition(window);
              if(int_mode_button.in_bounds(mouse_pos)) {
                int_mode_button.activate();
                float_mode_button.deactivate();
                float_mode = false;
              } else if(float_mode_button.in_bounds(mouse_pos)) {
                float_mode_button.activate();
                int_mode_button.deactivate();
                float_mode = true;
              } else if(add_mode_button.in_bounds(mouse_pos)) {
                add_mode_button.activate();
                del_mode_button.deactivate();
                del_mode = false;
              } else if(del_mode_button.in_bounds(mouse_pos)) {
                add_mode_button.deactivate();
                del_mode_button.activate();
                del_mode = true;
              } else if(undo_button.in_bounds(mouse_pos)) {
                undo = true;
              }
            }
            break;

        default: break;
      }
    } // end event loop

    // UPDATE
    

    //DRAW
    window.clear();

    window.draw(left_side_background);
    window.draw(right_side_background);

    int_mode_button.draw(window);
    float_mode_button.draw(window);

    window.draw(k_background);
    k_inc_button.draw(window);
    k_dec_button.draw(window);

    add_mode_button.draw(window);
    del_mode_button.draw(window);
    undo_button.draw(window);
    
    window.display();

  } // end game loop

  /*
  bool label_mode = false;
  int neighbors = 0;
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
