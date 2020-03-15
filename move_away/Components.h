#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "Harmony/sfml/gui/gui.h"

#include <cstdlib> // srand, rand

class Source;

class Point {

  float x_, y_;
  float vel_x_, vel_y_;
  float acc_x_, acc_y_;

  float charge_;

  sf::CircleShape visual_;

public:

  Point(int width, int height);

  void push_from(const Point&);
  void update(int width, int height);
  void draw(sf::RenderWindow& window) const { window.draw(visual_); }
};

class Source {

  float x_, y_;

  float charge_;

  sf::CircleShape visual_;

public:

  Source(float x, float y, float charge);
  void draw(sf::RenderWindow& window) const { window.draw(visual_); }

  //friend void Point::push_from(const Source&);
};

#endif
