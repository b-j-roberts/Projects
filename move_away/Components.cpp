#include "Components.h"

#include <cmath> // sqrt

Point::Point(int width, int height):
  x_(rand() % width), y_(rand() % height),
  vel_x_((rand() % 21 - 10) / 20.f), vel_y_((rand() % 21 - 10) / 20.f), // TO DO
  acc_x_(0), acc_y_(0),
 // charge_(2.f),
 // visual_(3, 6) {
  charge_(3.f),
  visual_(10) {
  visual_.setOrigin(10, 10);
  visual_.setPosition(x_, y_);
}

void Point::push_from(const Point& point) {
  float x_dist = x_ - point.x_;
  //if(x_dist > 50 || x_dist < -50) return;
  float y_dist = y_ - point.y_;
  //if(y_dist > 50 || y_dist < -50) return;
  float dist_sq = x_dist * x_dist + y_dist * y_dist;
  if(dist_sq < 1) return; // Prevent huge force
  float force = charge_ * point.charge_ / dist_sq;
  float dist = std::sqrt(dist_sq); 
  acc_x_ += force * (x_dist) / dist;
  acc_y_ += force * (y_dist) / dist;
}

void Point::update(int width, int height) {
  vel_x_ += acc_x_;
  vel_y_ += acc_y_;
  x_ += vel_x_;
  y_ += vel_y_;
  if(x_ > width) x_ -= width;
  else if(x_ < 0) x_ += width;
  if(y_ > height) y_ -= height;
  else if(y_ < 0) y_ += height;
  visual_.setPosition(x_, y_);
  acc_x_ = 0.f;
  acc_y_ = 0.f;
}

Source::Source(float x, float y, float charge):
  x_(x), y_(y),
  charge_(charge),
  visual_(6) {
  visual_.setOrigin(6, 6);
  visual_.setPosition(x, y);
}
