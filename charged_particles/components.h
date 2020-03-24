#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "Harmony/sfml/gui/gui.h"

// TO DO : Take frame rate into account

// A charged and massed point with an interface for being repelled/attracted by other points
// Sort of emulates a charged point in space 
class Point {

  float x_, y_;
  float vel_x_, vel_y_;
  float acc_x_, acc_y_;

  float mass_;
  float charge_;

  mutable sf::CircleShape visual_;

public:

  Point() = default;
  // params : pos_x, pos_y, vel_x, vel_y, mass, charge
  Point(int, int, float, float, float, float);
  Point(const Point&) = default;
  Point& operator=(const Point&) = default;
  
  float x() const { return x_; }
  float y() const { return y_; }

  // Apply force from point passed; build up x & y acc  
  void push_from(const Point&);
  // Update x_, y_, vel_x_, vel_y_ from acc calculated; apply Friction; & reset acc
  void update(size_t width, size_t height);
  // Draw visual_ to window with cloning for seemless overflow graphics
  void draw(sf::RenderWindow& window) const; 
};

#endif
