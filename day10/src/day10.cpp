#include <algorithm>
#include <assert.h>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

constexpr double EPSILON = 0.000001;

struct Point {
public:
  int x;
  int y;

  const bool operator==(const Point &obj) const {
    return obj.x == x && obj.y == y;
  }
  constexpr int manhattan_distance(Point p) {
    return abs(x - p.x) + abs(y - p.y);
  }
};

struct PointCmp {
private:
  Point center;

public:
  PointCmp(Point p) { center = p; };

  constexpr bool operator()(Point a, Point b) const {
    return a.manhattan_distance(center) < b.manhattan_distance(center);
  }
};

enum class Quadrant { I = 1, II = 2, III = 3, IV = 4 };

struct Vektor {
public:
  double a1;
  double a2;
  double norm;
  double euclidean_norm;
  Vektor(int arg1, int arg2) {
    norm = double(std::abs(arg1) + std::abs(arg2));
    a1 = (double)arg1 / norm;
    a2 = (double)arg2 / norm;
    euclidean_norm = std::sqrt(a1 * a1 + a2 * a2);
  }
  const bool operator==(const Vektor &obj) const {
    return std::abs(obj.a1 - a1) < EPSILON && std::abs(obj.a2 - a2) < EPSILON;
  }
  constexpr double scalar(const Vektor &v2) const {
    return a1 * v2.a1 + a2 * v2.a2;
  }

  constexpr Quadrant get_quadrant() const {
    if (a1 >= 0) {
      if (a2 <= 0) {
        return Quadrant::I;
      } else {
        return Quadrant::IV;
      }
    } else {
      if (a2 < 0) {
        return Quadrant::III;
      } else {
        return Quadrant::II;
      }
    }
  }

  constexpr double angle(const Vektor &v2) const {

    auto base_angle =
        std::acos((scalar(v2) / (this->euclidean_norm * v2.euclidean_norm)));
    auto quad = this->get_quadrant();
    if (quad == Quadrant::II || quad == Quadrant::III) {
      return M_PI + (M_PI - base_angle);
    } else {
      return base_angle;
    }
  }
};

// sort list by angle to vektor (1,0) => 0 degree
struct VektorCmp {
  bool operator()(Vektor a, Vektor b) {
    return a.angle(Vektor{0, -1}) < b.angle(Vektor{0, -1});
  }
};
namespace std {
template <> struct hash<Point> {

  std::size_t operator()(const Point &p) const {
    std::size_t val{0};
    val = 100000 * p.y + p.x;
    return val;
  }
};

template <> struct hash<Vektor> {

  std::size_t operator()(const Vektor &p) const {
    std::size_t val{0};
    val = 100000 * p.a1 + p.a2;
    return val;
  }
};
} // namespace std

std::ostream &operator<<(std::ostream &out, Point const &point) {
  out << "(" << point.x << "," << point.y << ")";
  return out;
}

std::vector<Point> string2vector(std::stringstream &ss) {
  std::vector<Point> vec;
  char a;
  int x = 0;
  int y = 0;
  while (ss >> a) {
    if (a == '#') {
      vec.push_back(Point{x, y});
    }
    x++;
    if (ss.peek() == '\n') {
      ss.ignore();
      y++;
      x = 0;
    }
  }
  return vec;
}

// part 2
int destroy(std::vector<Point> const &vec, Point center, int pos,
            bool verbose = false) {
  Point p;
  std::map<Vektor, std::vector<Point>, VektorCmp> vektor_map;
  // create ordered map by angle, starting with (1,0) as zero
  for (auto dst : vec) {
    if (center == dst)
      continue;
    auto key = Vektor(dst.x - center.x, dst.y - center.y);
    // angle doesnt exist create list for all elements with this angle
    if (vektor_map.find(key) == vektor_map.end()) {
      vektor_map[key] = std::vector<Point>();
      // vektor_map[key] = std::make_unique<std::vector<Point>>();
    }

    vektor_map[key].push_back(dst);
  }
  // return abs(a.x-center.x)+abs(a.y-center.y) <
  // abs(b.x-center.x)+abs(b.y-center.y);
  for (auto &[key, val] : vektor_map) {
    std::sort(val.begin(), val.end(), PointCmp(center));
  }
  int killed = 0;
  const auto ref = Vektor{0, -1};
  while (vektor_map.size() > 0) {
    for (auto &[key, val] : vektor_map) {
      if (val.size() > 0) {
        p = val.front();
        val.erase(val.begin());
        if (verbose) {
          std::cout << "killed " << p << " at step " << killed << " vektor "
                    << key.a1 << "," << key.a2 << " " << key.angle(ref) << "\n";
        }
        killed++;
      }
      if (killed == pos) {
        return p.x * 100 + p.y;
      }
    }
  }

  return p.x * 100 + p.y;
}

// part1
std::pair<int, Point> find_best_asteroid(std::vector<Point> const &vec) {
  int max_asteroids = 0;
  Point best_asteroid;
  for (auto src : vec) {
    std::unordered_set<Vektor, std::hash<Vektor>> vektor_set;
    for (auto dst : vec) {
      if (src == dst)
        continue;

      vektor_set.insert(Vektor(dst.x - src.x, dst.y - src.y));
    }
    // std::cout << src << " " << vektor_set.size() << " ";
    if (vektor_set.size() > max_asteroids) {
      max_asteroids = vektor_set.size();
      best_asteroid = src;
    }
  }
  return std::make_pair(max_asteroids, best_asteroid);
}

void tests() {
  auto p = Point{11, 13};
  assert(p.manhattan_distance(Point{11, 11}) == 2);
  assert(p.manhattan_distance(Point{11, 12}) == 1);
  std::ifstream infile("./input/input_10_test.txt");
  std::vector<Point> vec;
  std::stringstream ss;
  ss << infile.rdbuf();
  vec = string2vector(ss);
  auto [max_asteroids, denter] = find_best_asteroid(vec);
  assert(max_asteroids == 210);
  auto target_asteroid = destroy(vec, Point{11, 13}, 200);
  assert(target_asteroid == 802);
}

int main() {
  tests();
  std::ifstream infile("./input/input_10.txt");
  std::vector<Point> vec;
  std::stringstream ss;
  ss << infile.rdbuf();
  vec = string2vector(ss);
  auto [max_asteroids, center] = find_best_asteroid(vec);
  std::cout << "Part1: " << max_asteroids
            << "\n"; //<< " " << best_asteroid << " ";
  auto target_asteroid = destroy(vec, center, 200);
  std::cout << "Part2: " << target_asteroid;
  return 0;
}
