#include <assert.h>
#include <fmt/format.h>
#include <array>
#include <cctype>
#include <cstddef>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr bool VERBOSE { false };

enum class Direction {
	NORTH = 1, SOUTH = 2, WEST = 3, EAST = 4
};
std::array<Direction const, 4> const DIRECTIONS = { Direction::NORTH, Direction::SOUTH, Direction::WEST, Direction::EAST };

struct Point {
	int x;
	int y;

//	Point(int x_, int y_) :			x { x_ }, y { y_ } {	}
	bool operator==(const Point &obj) const {
		return obj.x == x && obj.y == y;
	}
	int manhattan_distance(Point p) const {
		return abs(x - p.x) + abs(y - p.y);
	}
};

std::ostream& operator<<(std::ostream &out, Point const &point) {
	out << "(" << point.x << "," << point.y << ")";
	return out;
}

namespace std {
template<>
struct hash<Point> {
	std::size_t operator()(const Point &p) const {
		std::size_t val { 0 };
		val = 100000 * p.y + p.x;
		return val;
	}
};

template<typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
	std::size_t operator()(const std::pair<T1, T2> &pair) const {
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);;
	}
};
}  // namespace std

struct PointCost {
	Point point;
	int cost;
};

std::ostream& operator<<(std::ostream &out, PointCost const &p) {
	out << p.point << ":" << p.cost << ";";
	return out;
}

struct ExploreElement {
	std::unordered_set<char> keys;
	std::unordered_set<char> locks;
	Point cur_point;
	int steps;
};

struct LessThanExploreElement {
	bool operator()(const ExploreElement &lhs, const ExploreElement &rhs) const {
		return lhs.steps < rhs.steps;
	}
};
struct PointCmp {
private:
	Point center;

public:
	PointCmp(Point p) {
		center = p;
	}
	;

	bool operator()(Point a, Point b) const {
		return a.manhattan_distance(center) < b.manhattan_distance(center);
	}
};

template<typename T>
constexpr auto to_type(T e) {
	return static_cast<typename std::underlying_type<T>::type>(e);
}

Direction point_to_dir(const Point &src, const Point &dst) {
	int x_diff { dst.x - src.x };
	int y_diff { dst.y - src.y };
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
		return Point { p1.x, p1.y - 1 };
	case Direction::EAST:
		return Point { p1.x + 1, p1.y };
	case Direction::SOUTH:
		return Point { p1.x, p1.y + 1 };
	case Direction::WEST:
		return Point { p1.x - 1, p1.y };
	default:
		break;
	}
	std::cerr << "Unkown direction " << to_type(dir) << "\n";
	return Point { -1, -1 };
}

bool is_key(char c) {
	return c >= 'a' && c <= 'z';
}

bool is_lock(char c) {
	return c >= 'A' && c <= 'Z';
}

std::vector<Point> get_avail_neighbors(const std::unordered_map<Point, char> &point_map, const Point &cur_point) {
	std::vector<Point> result { };
	for (auto dir : DIRECTIONS) {
		auto next = dir_to_point(cur_point, dir);
		auto found = point_map.find(next);
		if (found != point_map.end()) {
			result.push_back(found->first);
		}
	}
	return result;
}

int update_costs(std::unordered_map<Point, int> &cost_map, const Point &neighbor, const int cost) {
	int new_cost { cost + 1 };
	auto found = cost_map.find(neighbor);
	if (found != cost_map.end()) {
		if (new_cost < found->second) {
			found->second = new_cost;
		} else {
			new_cost = found->second;
		}
	} else {
		cost_map.insert( { neighbor, new_cost });
	}
	return new_cost;
}

std::vector<PointCost> get_next_moves_pre(const std::unordered_map<Point, char> &point_map,
		const std::unordered_map<Point, std::vector<PointCost>> &reach_map, const std::unordered_map<char, Point> &POI_map, const Point &point,
		const std::unordered_set<char> &keys, const std::unordered_set<char> &locks, std::vector<PointCost> &result,
		std::unordered_set<char> &visited) {
	auto next_moves = reach_map.find(point)->second;

	while (!next_moves.empty()) {
		auto [dst_point, dst_costs] = next_moves.front();
		next_moves.erase(next_moves.begin());
		auto dst_type = point_map.find(dst_point)->second;
		if (visited.find(dst_type) != visited.end()) {
			continue;
		} else {
			visited.insert(dst_type);
		}
		// heck if we already visited this point
		if (keys.find(dst_type) != keys.end() || locks.find(dst_type) != locks.end()) {
			auto next_next = reach_map.find(dst_point)->second;

			for (auto [nn_point, nn_costs] : next_next) {
				next_moves.push_back(PointCost { nn_point, nn_costs + dst_costs });
			}
		} else {
			if (is_lock(dst_type)) {
				if (locks.find(dst_type) == locks.end()) {
					// found a  new lock where we have the key
					auto need_key = std::tolower(dst_type);
					if (keys.find(need_key) != keys.end()) {
						result.push_back(PointCost { dst_point, dst_costs });
					}
				}
			} else {
				result.push_back(PointCost { dst_point, dst_costs });
			}
		}
	}
	return result;
}

std::vector<PointCost> get_next_moves(const std::unordered_map<Point, char> &point_map, const Point &start_point, const std::unordered_set<char> keys,
		const std::unordered_set<char> locks) {
	std::deque<PointCost> deq;
	std::vector<PointCost> result;
	deq.push_back( { start_point, 0 });
	std::unordered_set<Point> visited { };
	visited.insert(start_point);
	std::unordered_map<Point, int> cost_map { };
	cost_map.insert( { start_point, 0 });
	while (!deq.empty()) {
		auto [cur_point, cost] = deq.front();
		deq.pop_front();
		auto neighbors = get_avail_neighbors(point_map, cur_point);
		for (auto neighbor : neighbors) {
			auto found = point_map.find(neighbor);
			assert(found != point_map.end());
			auto new_costs = update_costs(cost_map, neighbor, cost);
			if (is_key(found->second)) {
				if (keys.find(found->second) == keys.end() && visited.find(neighbor) == visited.end())
					result.push_back( { neighbor, new_costs });
				// else
				deq.push_back( { neighbor, new_costs });
			} else if (is_lock(found->second)) {
				if (locks.find(found->second) == locks.end()) {
					// found a  new lock where we have the key
					// auto need_key = std::tolower(found->second);
					//   if (keys.find(need_key) != keys.end()) {
					if (visited.find(neighbor) == visited.end())
						result.push_back( { neighbor, new_costs });
					//    }
				} else {
					// already solved this lock, treat is as an empty space
					deq.push_back( { neighbor, new_costs });
				}

			} else if (visited.find(neighbor) == visited.end()) {
				deq.push_back( { neighbor, new_costs });
			}
			visited.insert(neighbor);
		}
	}
	return result;
}

int find_min_steps(const std::unordered_map<Point, char> &point_map, std::unordered_set<char> &keys_to_find,
		const std::unordered_map<Point, std::vector<PointCost>> &reach_map, const std::unordered_map<char, Point> &POI_map, Point start) {
	std::priority_queue<ExploreElement, std::vector<ExploreElement>, LessThanExploreElement> deq { };
	std::unordered_set<char> init_keys;
	init_keys.reserve(27);
	std::unordered_set<char> init_locks;
	init_locks.reserve(27);
	ExploreElement el { init_keys, init_locks, start, 0 };
	deq.push(el);
	std::vector<PointCost> next_moves;
	std::unordered_set<char> visited;
	int min_steps { 6768 };  // 6768 too high INT32_MAX
	while (!deq.empty()) {
		auto [keys, locks, cur_point, steps] = deq.top();
		deq.pop();

		if (keys.size() == keys_to_find.size()) {
			if (steps < min_steps) {
				min_steps = steps;
				std::cout << min_steps << " \n";
			}

		} else if (steps > min_steps) {  // 6768

			// do nothing
		} else {
			next_moves.clear();
			visited.clear();
			//   auto next_moves = get_next_moves(pmap, cur_point, keys, locks);
			next_moves = get_next_moves_pre(point_map, reach_map, POI_map, cur_point, keys, locks, next_moves, visited);
			//    std::cout << cur_point << " :";
			for (auto [next_point, next_cost] : next_moves) {
				//    std::cout << next_point << ";";

				// std::unordered_map<Point, char> new_pmap = pmap;
				auto found = point_map.find(next_point);
				assert(found != point_map.end());
				std::unordered_set<char> new_keys = keys;
				std::unordered_set<char> new_locks = locks;
				if (is_key(found->second)) {
					new_keys.insert(found->second);
				}
				if (is_lock(found->second)) {
					new_locks.insert(found->second);
				}
				// new_pmap[next_point] = '.';
				auto next_steps = steps + next_cost;
				ExploreElement next_el { new_keys, new_locks, next_point, next_steps };
				deq.push(next_el);
			}
			//  std::cout << "\n";
		}
	}

	return min_steps;
}

Point parse_input(std::string const &filename, std::unordered_map<Point, char> &point_map, std::unordered_map<char, Point> &POI_map,
		std::unordered_set<char> &all_keys, std::unordered_set<char> &all_locks) {
	std::ifstream infile(filename);
	if (not infile.is_open()) {
		std::cerr << "can't open file " << filename;
	}
	std::stringstream ss { };
	Point start_point { };
	ss << infile.rdbuf();
	auto string = ss.str();
	int x { 0 };
	int y { 0 };
	for (char const &a : string) {
		switch (a) {
		case '#':
			x++;
			break;
		case '.':
			point_map.insert( { Point { x, y }, a });
			x++;
			break;
		case '@':
			POI_map.insert( { a, Point { x, y } });
			point_map.insert( { Point { x, y }, '.' });
			start_point = Point { x, y };
			x++;
			break;
		case '\n':
			y++;
			x = 0;
			break;
		default:
			point_map.insert( { Point { x, y }, a });
			if (a >= 'a' && a <= 'z') {
				all_keys.insert(a);
				POI_map.insert( { a, Point { x, y } });
			}
			if (a >= 'A' && a <= 'Z') {
				all_locks.insert(a);
				POI_map.insert( { a, Point { x, y } });
			}
			x++;
			break;
		}
		std::cout << a;
	}
	std::cout << "\n";
	return start_point;
}

void print_reach_map(std::unordered_map<Point, std::vector<PointCost>> const &reach_map ,std::unordered_map<Point, char> &point_map ){
	for (auto [origin, dsts] : reach_map) {
		std::cout << origin << " " << point_map[origin] << "-";
		for (auto dst : dsts) {
			std::cout << point_map[dst.point] << " " << dst << " ";
		}
		std::cout << "\n";
	}
}

std::unordered_map<Point, std::vector<PointCost>> create_reach_map(const std::unordered_map<Point, char> &point_map,
		std::unordered_map<char, Point> &POI_map) {
	std::unordered_set<char> keys { };
	std::unordered_set<char> locks { };
	std::unordered_map<Point, std::vector<PointCost>> reach_map { };
	for (auto [src_key, src_point] : POI_map) {
		reach_map.insert( { src_point, std::vector<PointCost> { } });
		keys.clear();
		if (is_key(src_key)) {
			keys.insert(src_key);
		}
		auto next_moves = get_next_moves(point_map, src_point, keys, locks);
		for (auto next_move : next_moves) {
			reach_map.find(src_point)->second.push_back(next_move);
		}
	}
	return reach_map;
}

#ifndef TESTING
int main() {
	system("pwd");
	constexpr auto filename { "../day18/input/input_18_1.txt" };
	std::unordered_map<Point, char> point_map;
	std::unordered_set<char> keys_to_find;
	std::unordered_set<char> all_locks;
	std::unordered_map<char, Point> POI_map;
	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
	auto reach_map = create_reach_map(point_map, POI_map);
	for (auto [origin, dsts] : reach_map) {
		std::cout << origin << " " << point_map[origin] << "-";
		for (auto dst : dsts) {
			std::cout << point_map[dst.point] << " " << dst << " ";
		}
		std::cout << "\n";
	}

	auto part1 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);
	std::cout << "Part 1: " << part1 << "\n";

	std::cout << "Part 2: " << "" << "\n";
	return 0;
}
#endif
