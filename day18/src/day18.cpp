#include <array>
#include <assert.h>
#include <cctype>
#include <deque>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr bool VERBOSE{false};

enum class Direction { NORTH = 1, SOUTH = 2, WEST = 3, EAST = 4 };
std::array<Direction const, 4> DIRECTIONS = {Direction::NORTH, Direction::SOUTH,
                                             Direction::WEST, Direction::EAST};
struct Point {
public:
  int x;
  int y;

  const bool operator==(const Point &obj) const {
    return obj.x == x && obj.y == y;
  }
  int manhattan_distance(Point p) { return abs(x - p.x) + abs(y - p.y); }
};

struct PointCost {
  Point point;
  int cost;
};

struct PointCmp {
private:
  Point center;

public:
  PointCmp(Point p) { center = p; };

  bool operator()(Point a, Point b) const {
    return a.manhattan_distance(center) < b.manhattan_distance(center);
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
} // namespace std

std::ostream &operator<<(std::ostream &out, Point const &point) {
  out << "(" << point.x << "," << point.y << ")";
  return out;
}

template <typename T> constexpr auto to_type(T e) {
  return static_cast<typename std::underlying_type<T>::type>(e);
}

Direction point_to_dir(const Point &src, const Point &dst) {
  int x_diff{dst.x - src.x};
  int y_diff{dst.y - src.y};
  assert(std::abs(x_diff) + std::abs(y_diff) == 1);
  if (x_diff == -1) {
    return Direction::WEST;
  }
  if (x_diff == 1) {
    return Direction::EAST;
  }
  if (y_diff == -1) {
    return Direction::NORTH;
  }
  if (y_diff == 1) {
    return Direction::SOUTH;
  }
  std::cerr << "Point are not neighbors\n";
  return Direction::NORTH;
}

Point dir_to_point(const Point &p1, Direction dir) {
  switch (dir) {
  case Direction::NORTH:
    return Point{p1.x, p1.y - 1};
  case Direction::EAST:
    return Point{p1.x + 1, p1.y};
  case Direction::SOUTH:
    return Point{p1.x, p1.y + 1};
  case Direction::WEST:
    return Point{p1.x - 1, p1.y};
  default:
    break;
  }
  std::cerr << "Unkown direction " << to_type(dir) << "\n";
  return Point{-1, -1};
}

bool is_key(char c) { return c >= 'a' && c <= 'z'; }

bool is_lock(char c) { return c >= 'A' && c <= 'Z'; }

std::vector<Point>
get_avail_neighbors(const std::unordered_map<Point, char> &point_map,
                    const Point &cur_point) {
  std::vector<Point> result{};
  for (auto dir : DIRECTIONS) {
    auto next = dir_to_point(cur_point, dir);
    auto found = point_map.find(next);
    if (found != point_map.end()) {
      result.push_back(found->first);
    }
  }
  return result;
}

int update_costs(std::unordered_map<Point, int> &cost_map,
                 const Point &neighbor, const int cost) {
  int new_cost{cost + 1};
  auto found = cost_map.find(neighbor);
  if (found != cost_map.end()) {
    if (new_cost < found->second) {
      found->second = new_cost;
    } else {
      new_cost = found->second;
    }
  } else {
    cost_map.insert({neighbor, new_cost});
  }
  return new_cost;
}

std::vector<PointCost>
get_next_moves(const std::unordered_map<Point, char> &point_map,
               const Point &start_point, std::unordered_set<char> keys) {
  std::deque<PointCost> deq;
  std::vector<PointCost> result;
  deq.push_back({start_point, 0});
  std::unordered_set<Point> visited{};
  std::unordered_map<Point, int> cost_map{};
  cost_map.insert({start_point, 0});
  while (!deq.empty()) {
    auto [cur_point, cost] = deq.front();

    auto neighbors = get_avail_neighbors(point_map, cur_point);
    for (auto neighbor : neighbors) {
      auto found = point_map.find(neighbor);
      assert(found != point_map.end());
      auto new_costs = update_costs(cost_map, neighbor, cost);
      if (is_key(found->second)) {
        // found a key

        result.push_back({neighbor, new_costs});

      } else if (is_lock(found->second)) {
        // found a lock where we have the key
        auto need_key = std::tolower(found->second);
        if (keys.find(need_key) != keys.end()) {
          result.push_back({neighbor, new_costs});
        }
      } else if (visited.find(neighbor) != visited.end()) {
        deq.push_back({neighbor, new_costs});
      }
      visited.insert(neighbor);
    }
  }
  return result;
}

int find_min_steps(std::unordered_map<Point, char> &point_map,
                   std::unordered_set<char> &keys_to_find, Point start) {

  return 0;
}

Point parse_input(std::string filename,
                  std::unordered_map<Point, char> &point_map,
                  std::unordered_set<char> &all_keys) {
  std::ifstream infile(filename);
  std::stringstream ss{};
  Point start_point{};
  ss << infile.rdbuf();
  auto string = ss.str();
  char a{};
  int x{0};
  int y{0};
  for (char const &a : string) {
    switch (a) {
    case '#':
      x++;
      break;
    case '.':
      point_map.insert({Point{x, y}, a});
      x++;
      break;
    case '@':
      point_map.insert({Point{x, y}, '.'});
      start_point = Point{x, y};
      x++;
      break;
    case '\n':
      y++;
      x = 0;
      break;
    default:
      point_map.insert({Point{x, y}, a});
      if (a >= 'a' && a <= 'z') {
        all_keys.insert(a);
      }
      if (a >= 'A' && a <= 'Z') {
      }
      x++;
      break;
    }
    std::cout << a;
  }
  std::cout << "\n";
  return start_point;
}

int main() {
  auto filename = "./input/input_18_test.txt";
  std::unordered_map<Point, char> point_map;
  std::unordered_set<char> keys_to_find;
  auto start = parse_input(filename, point_map, keys_to_find);

  std::cout << "Part 1: " << find_min_steps(point_map, keys_to_find, start)
            << "\n";

  std::cout << "Part 2: "
            << ""
            << "\n";
  return 0;
}
