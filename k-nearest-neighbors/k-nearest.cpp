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

// k_nearest returns a vector of the index of the k nearest neighbors in the given vector to a point passed
template<typename T>
const std::vector<int> k_nearest(const std::vector<Point<T>>& points, const Point<T>& point, size_t k) {  

  // Get distances between all points and passed point
  std::vector<T> dist(points.size());
  std::transform(points.begin(), points.end(), dist.begin(), [&](auto& p) { return metric(point, p); });

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

// Does same thing as other k_nearest (see above) but for a point inside of the vector at position idx
template<typename T>
const std::vector<int> k_nearest(const std::vector<Point<T>>& points, size_t idx, size_t k) {
   auto ret = k_nearest(points, points[idx], k + 1);
   ret.erase(std::remove(ret.begin(), ret.end(), idx), ret.end());
   return ret;
}


int main() {

  bool float_mode = false;
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
