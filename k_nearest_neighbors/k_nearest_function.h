#ifndef K_NEAR_FUNC_INCLUDED
#define K_NEAR_FUNC_INCLUDED

#include <string>
#include <vector>

#include <algorithm> // min, transform, sort
#include <iterator> // back_inserter

// 2D point class with label
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

// METRICS

// Metrics return the distance between the two point objects given
template<typename T>
T metric(const Point<T>& p1, const Point<T>& p2);

// Helper : Holds position and value of element of a vector
template<typename T>
struct pos_holder {
  T value;
  size_t pos;
  pos_holder(T val, int i): value(val), pos(i) { }
};

// k_nearest returns a vector of the index of the 'k' nearest points to 'point' in 'points'
template<typename T>
std::vector<int> k_nearest(const std::vector<Point<T>>& points, const Point<T>& point, size_t k) {
  // Group distances btw points & point with vector index of points then sort by distance
  std::vector<pos_holder<T>> pos;
  // OPTIMIZE : sqrt is increasing function on domain so doing metric on float is kinda wasting
  //            computational power for this part since k_nearest will have lowest metric sqared too
  std::transform(points.begin(), points.end(), std::back_inserter(pos),
                 [&, i = 0](const auto& p) mutable { return pos_holder<T>(metric(point, p), i++); });
  // OPTIMIZE : Partial sort?
  std::sort(pos.begin(), pos.end(), [](const pos_holder<T>& a, const pos_holder<T>& b) -> bool {
    return a.value < b.value;
  });

  // Take k closest values and return the index of them
  std::vector<int> ret;
  std::transform(pos.begin(), pos.begin() + std::min(k, pos.size()), std::back_inserter(ret),
                 [](const auto& holder){ return holder.pos; });
  return ret;
}

// Same as other k_nearest (see above) but for a point inside of the vector at position 'idx'
template<typename T>
std::vector<int> k_nearest(const std::vector<Point<T>>& points, size_t idx, size_t k) {
  auto ret = k_nearest(points, points[idx], k + 1);
  ret.erase(std::remove(ret.begin(), ret.end(), idx), ret.end());
  return ret;
}

#endif
