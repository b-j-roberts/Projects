#include <iostream> // Debugging headers

#include <math.h>
#include <string>
#include <vector>
#include <algorithm>

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

// k_nearest returns a vector of the index of the k nearest neighbors in the given vector to a point passed
template<typename T>
const std::vector<int> k_nearest(const std::vector<Point<T>>& points, const Point<T>& point, int k) {  

  std::vector<T> dist(points.size());
  std::transform(points.begin(), points.end(), dist.begin(), [&](auto& p) { return metric(point, p); });

  // HOW TO OBTAIN INDEX AFTER SORT ? 

  std::vector<int> ret;
  return ret;
}

// TO DO : Make a function like above but using a point inside of the vector, so we just pass its index ?
//           Should we ignore this point in the search for the k nearest neighbors ? maybe add 1 to k ?

int main() {
 
  std::vector<Point<float>> pi = { Point<float>(2.2, 2.1), Point<float>(2.1, 4.0), Point<float>(3.4, 5.2) };
  auto r = k_nearest(pi, pi[0], 1);

  return 0;
}


// FUTURE REMARKS :
//   Include a circle around K-nearest in the visual
//   allow placing of labels and then an identifier based on k-nearest (can change k), on a tie pick the nearest neighbor,
//     on a tie for nearest neighbors include all ties too (highlight and make it pop up with a note) 
//     on a tie for nearest neighbor on classification (ie ties in number and then ties in finding nearest (label) then pop up 50 / 50
