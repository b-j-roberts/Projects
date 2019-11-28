#include <SFML/Graphics.hpp>

#include <iostream> // Debug

#include <stdio.h>  // NULL
#include <stdlib.h> // srand, rand
#include <time.h>   // time

#include <cmath>    // pow, sqrt

#include <array>
#include <vector>

int bound_x_high, bound_y_high;

float dist(int x_1, int y_1, int x_2, int y_2) {
  return sqrt(pow(x_1 - x_2, 2) + pow(y_1 - y_2, 2));
}

class Point {

  int x_, y_;
  float vel_x_, vel_y_;

  int red_shift_;

  sf::CircleShape visual_;

  bool within_distance_scattered(int x, int y) {
  
    // scalable parameters!
    static const int distance = 100;
    static const int rand_scale = 30;

    const int x_offset = rand() % (rand_scale * 2) - rand_scale;
    const int y_offset = rand() % (rand_scale * 2) - rand_scale; 

    if(dist(x_offset + x_, y_offset + y_, x, y) < distance) return true;
    else return false;

  }

public:

  Point() { }
  Point(int width, int height): x_(rand() % width), y_(rand() % height), 
                                vel_x_((rand() % 100 - 50) * .01f), 
                                vel_y_((rand() % 100 - 50) * .01f), 
                                red_shift_(0),
                                visual_(5) { 
                             
    visual_.setPosition(x_, y_);

  }

  int x() { return x_; }
  int y() { return y_; }

  void draw(sf::RenderWindow& window) {

    x_ += vel_x_;
    y_ += vel_y_;

    if(x_ < 0) x_ += bound_x_high;
    else if(x_ > bound_x_high) x_ -= bound_x_high;

    if(y_ < 0) y_ += bound_y_high;
    else if(y_ > bound_y_high) y_ -= bound_y_high;

    visual_.setPosition(x_, y_);
    if(red_shift_ > 255) red_shift_ = 255;
    else if(red_shift_ < 0) red_shift_ = 0;
    visual_.setFillColor(sf::Color(255, 255 - red_shift_, 255 - red_shift_, 255));

    window.draw(visual_);

  }

  // update Point based on position of point ( x , y ) 
  void update(int x, int y) {
    
    static float max_vel = 2;
    static float min_vel = -2;

    // If player near ( run ! )
    if(within_distance_scattered(x, y)) {
      if(x < x_) vel_x_ += rand() % 100 * .01f;
      else       vel_x_ -= rand() % 100 * .01f;

      if(y < y_) vel_y_ += rand() % 100 * .01f;
      else       vel_y_ -= rand() % 100 * .01f;

      red_shift_ += 40;

    } else { // If player far ( roam . )
      int r_1 = rand() % 2;
      int r_2 = rand() % 2;

      if(r_1) vel_x_ += rand() % 100 * .001f;
      else vel_x_ -= rand() % 100 * .001f;

      if(r_2) vel_y_ += rand() % 100 * .001f;
      else vel_y_ -= rand() % 100 * .001f;

      red_shift_ -= 1;

    }

    if(vel_x_ > max_vel) vel_x_ = max_vel;
    else if(vel_x_ < min_vel) vel_x_ = min_vel;

    if(vel_y_ > max_vel) vel_y_ = max_vel;
    else if(vel_y_ < min_vel) vel_y_ = min_vel;


  }

};

class Player {

  sf::CircleShape visual_;

public:

  Player(): visual_(20) { visual_.setFillColor(sf::Color(255, 0, 0, 255)); }

  void draw(sf::RenderWindow& window) { // TO DO

    window.draw(visual_);

  }

  void update(int x, int y) {
    visual_.setPosition(x, y);
  }

};

class Hole {

  int x_, y_;
  sf::CircleShape visual_;

public:

  Hole(int x, int y): x_(x), y_(y), visual_(10) {
    visual_.setPosition(x_, y_);
    visual_.setFillColor(sf::Color(20, 0, 150, 255));
  }

  void draw(sf::RenderWindow& window) {
    window.draw(visual_);
  }

  int x() { return x_; }
  int y() { return y_; }

};

int main() {

  int width, height;
  int x = 0;
  int y = 0;
  srand(time(NULL));


// Window
  sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
  sf::RenderWindow window(desktop, "Move Away!");

  window.setFramerateLimit(60);

  //Window Size & Scale
  const sf::Vector2u window_size(window.getSize());
  const float x_scale(window_size.x * 1.f / window_size.y);
  width = window_size.x;
  height = window_size.y;
  bound_x_high = width;
  bound_y_high = height;


// Create points
  const size_t num_of_points = 1000;

  std::vector<Point> points;
  for(size_t i = 0; i < num_of_points; ++i) {
    points.emplace_back(width, height);
  }

  Player person;
  std::vector<Hole> holes;

// Game Loop

  sf::Event event;

  while(window.isOpen()) {

    // Input
    while(window.pollEvent(event)) {
      switch(event.type) {
      
        case sf::Event::KeyPressed:
          switch(event.key.code) {
            case sf::Keyboard::Escape:
              window.close();
              break;
            case sf::Keyboard::Space:
              holes.emplace_back(x, y);
              break;
            default: break;
          }
          break; // key pressed

          case sf::Event::Closed:
            window.close();
            break;

        default: break;
      }
    } // end event loop
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      y -= 2;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      x -= 2;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      y += 2;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      x += 2;
    }

    // Update

/*    
    for(auto& point : points) {
      for(auto& point_ : points) {
        point.update(point_.x(), point_.y());
      }
      point.update(x, y);
    }
    person.update(x, y);
  */  

    for(auto& point_ : points) {
      point_.update(x, y);
      for(auto hole : holes) {
        point_.update(hole.x(), hole.y());
      }
    }
    person.update(x, y);

    // Draw
    window.clear(sf::Color(0, 0, 0, 255));

    for(auto& point : points) {
      point.draw(window);
    }
    for(auto& hole : holes) {
      hole.draw(window);
    }
    person.draw(window);

    window.display();

  } // end game loop

  return 0;


}
