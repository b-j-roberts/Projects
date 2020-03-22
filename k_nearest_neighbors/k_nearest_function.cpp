#include "k_nearest_function.h"

#include <cmath> // abs, sqrt

// City Block Metric
template<>
int metric<int>(const Point<int>& p1, const Point<int>& p2) {
  return abs( p1.x() - p2.x() ) + abs( p1.y() - p2.y() );
}

// Euclidean Metric
template<>
float metric<float>(const Point<float>& p1, const Point<float>& p2) {
  static auto sq = [](float x){ return x * x; };
  return sqrt( sq(p1.x() - p2.x()) + sq(p1.y() - p2.y()) );
}
