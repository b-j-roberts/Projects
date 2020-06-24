#include "components.h"

#include <cmath> // sqrt, abs

Point::Point(int pos_x, int pos_y, float vel_x, float vel_y, float mass, float charge):
  x_(static_cast<float>(pos_x)), y_(static_cast<float>(pos_y)),
  vel_x_(vel_x), vel_y_(vel_y),
  acc_x_(0), acc_y_(0),
  mass_(mass),
  charge_(charge),
  visual_(5 * mass) {
  visual_.setOrigin(5 * mass, 5 * mass);
  visual_.setPosition(x_, y_);
  // Function to linearly scale color with charge ( red = negative & blue = positive charge )
  visual_.setFillColor(sf::Color(static_cast<unsigned char>((charge < 0) * (-35 * charge + 135)), 0,
                                 static_cast<unsigned char>((charge > 0) * (35 * charge + 135))));
  // NOTE : 0 charge particles are "invisible" ( ie black )
}

void Point::push_from(const Point& point) {
  const int window_size = 1000; // TO DO : Hardcoded window size

  // Find shortest x & y distance between this & 'point' ( including wraping around )
  float x_dist = x_ - point.x_;
  if(std::abs(x_dist) > window_size / 2) // Do wrapped forces
    x_dist = x_ - (point.x_ + (x_dist < 0 ? -1 : 1) * window_size);
  float y_dist = y_ - point.y_;
  if(std::abs(y_dist) > window_size / 2) // Do wrapped forces
    y_dist = y_ - (point.y_ + (y_dist < 0 ? -1 : 1) * window_size);

  // Compute force, with max allowed force & constant force scale factor 
  float dist_sq = x_dist * x_dist + y_dist * y_dist;
  if(dist_sq < 0.01f && dist_sq > -0.01f) return; // Prevent div by 0
  constexpr float force_scale = 20.f;
  float force = force_scale * charge_ * point.charge_ / dist_sq;
  // Minmax force
  constexpr float max_force = 2.f;
  if(force > max_force) force = max_force;
  else if(force < -1.f * max_force) force = -1.f * max_force;

  // Accumulate force to acceleration
  float dist = std::sqrt(dist_sq); 
  acc_x_ += (force / mass_) * (x_dist) / dist;
  acc_y_ += (force / mass_) * (y_dist) / dist;
}

void Point::update(float width, float height) {
  // Update velocity
  vel_x_ += acc_x_;
  vel_y_ += acc_y_;
  constexpr float friction_factor = .99f;
  vel_x_ *= friction_factor;
  vel_y_ *= friction_factor;

  // Update position ( with wrapping )
  x_ += vel_x_;
  y_ += vel_y_;
  if(x_ > width) x_ -= width;
  else if(x_ < 0) x_ += width;
  if(y_ > height) y_ -= height;
  else if(y_ < 0) y_ += height;

  // Reset acc
  acc_x_ = 0.f;
  acc_y_ = 0.f;
}

void Point::draw(sf::RenderWindow& window) const { 
  const int window_size = 1000; // TO DO : Hardcoded window size 
  // Draw visual_ to window
  visual_.setPosition(x_, y_);
  window.draw(visual_);

  // Draw clone of visual_ to window in wrapped position if necessary ( for seemless wrap drawing )
  bool clone = false;
  sf::Vector2f clone_position(x_, y_);
  constexpr int clone_sensitivity = 30; // TO DO : Change these values if the points get larger?
  // Clones x_pos
  if(x_ < clone_sensitivity) {
    clone_position.x += window_size;
    clone = true;
  } else if(x_ > window_size - clone_sensitivity) {
    clone_position.x -= window_size;
    clone = true;
  }
  // Clones y_pos
  if(y_ < clone_sensitivity) {
    clone_position.y += window_size;
    clone = true;
  } else if(y_ > window_size - clone_sensitivity) {
    clone_position.y -= window_size;
    clone = true;
  }
  // Draw if needed
  if(clone) {
    visual_.setPosition(clone_position);
    window.draw(visual_);
  }
}
