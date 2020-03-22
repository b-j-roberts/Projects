#include "Components.h"

#include <cmath> // sqrt

Point::Point(int pos_x, int pos_y, float vel_x, float vel_y, float mass, float charge):
  x_(pos_x), y_(pos_y),
  vel_x_(vel_x), vel_y_(vel_y),
  acc_x_(0), acc_y_(0),
  mass_(mass),
  charge_(charge),
  visual_(5 * mass) {
  visual_.setOrigin(5 * mass, 5 * mass);
  visual_.setPosition(x_, y_);
  visual_.setFillColor(sf::Color(static_cast<unsigned char>((charge < 0) * (-35 * charge + 135)), 0,
                                 static_cast<unsigned char>((charge > 0) * (35 * charge + 135))));
  // NOTE : 0 charge particles are invisible
}

void Point::push_from(const Point& point) {
  int window_size = 1000;
  float x_dist = x_ - point.x_;
  if(std::abs(x_dist) > window_size / 2) // Do overlapping forces
    x_dist = x_ - (point.x_ + (x_dist < 0 ? -1 : 1) * window_size);
  float y_dist = y_ - point.y_;
  if(std::abs(y_dist) > window_size / 2) // Do overlapping forces
    y_dist = y_ - (point.y_ + (y_dist < 0 ? -1 : 1) * window_size);
  float dist_sq = x_dist * x_dist + y_dist * y_dist;
  if(dist_sq < 0.01f && dist_sq > -0.01f) return; // Prevent div by 0
  float force = 20.f * charge_ * point.charge_ / dist_sq;
  const float max_force = 2.f;
  // TO DO : minmax?
  // Minmax force
  if(force > max_force) force = max_force;
  else if(force < -1.f * max_force) force = -1.f * max_force;
  float dist = std::sqrt(dist_sq); 
  // Accumulate to acceleration
  acc_x_ += (force / mass_) * (x_dist) / dist;
  acc_y_ += (force / mass_) * (y_dist) / dist;
}

void Point::update(size_t width, size_t height) {
  vel_x_ += acc_x_;
  vel_y_ += acc_y_;
  vel_x_ *= .99; // Friction
  vel_y_ *= .99; // Friction
  x_ += vel_x_;
  y_ += vel_y_;
  if(x_ > width) x_ -= width;
  else if(x_ < 0) x_ += width;
  if(y_ > height) y_ -= height;
  else if(y_ < 0) y_ += height;
  acc_x_ = 0.f;
  acc_y_ = 0.f;
}

void Point::draw(sf::RenderWindow& window) const { 
  visual_.setPosition(x_, y_);
  window.draw(visual_);
  int window_size = 1000; // TO DO : Search for and fix window sizes throughout code
  bool clone = false;
  sf::Vector2f clone_position(x_, y_);
  if(x_ < 30) {
    clone_position.x += window_size;
    clone = true;
  } else if(x_ > window_size - 30) {
    clone_position.x -= window_size;
    clone = true;
  }
  if(y_ < 30) {
    clone_position.y += window_size;
    clone = true;
  } else if(y_ > window_size - 30) {
    clone_position.y -= window_size;
    clone = true;
  }
  if(clone) {
    visual_.setPosition(clone_position);
    window.draw(visual_);
  }
}

/*
Source::Source(float x, float y, float charge):
  x_(x), y_(y),
  charge_(charge),
  visual_(6) {
  visual_.setOrigin(6, 6);
  visual_.setPosition(x, y);
}
*/
