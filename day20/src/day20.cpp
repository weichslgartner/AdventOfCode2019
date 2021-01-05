#include <algorithm>
#include <array>

#include <cassert>
#include <cctype>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <tuple>
#include <fstream>
#include <functional>
#include <deque>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct Point {
	int x;
	int y;
	bool operator==(const Point &obj) const {
		return obj.x == x && obj.y == y;
	}
	int manhattan_distance(Point p) const {
		return abs(x - p.x) + abs(y - p.y);
	}
};

namespace std {
template<>
struct hash<Point> {
	std::size_t operator()(const Point &p) const {

		return (std::hash<int> { }(p.x)) + (std::hash<int> { }(p.y) << 16);
	}
};
}

namespace fmt {
template<>
struct formatter<Point> {
	template<typename ParseContext>
	constexpr auto parse(ParseContext &ctx) const {
		return ctx.begin();
	}
	template<typename FormatContext>
	auto format(Point const &p, FormatContext &ctx) const {
		return fmt::format_to(ctx.out(), "{},{}", p.x, p.y);
	}
};
}

std::vector<Point> get_neighbors(Point const &p, std::unordered_map<Point, std::string> const &point_map) {
	std::vector<Point> points { };
	points.reserve(4U);
	for (auto const &point : { Point { p.x, p.y - 1 }, Point { p.x, p.y + 1 }, Point { p.x - 1, p.y }, Point { p.x + 1, p.y } }) {
		auto const it = point_map.find(point);
		if (it != point_map.end()) {
			points.push_back(it->first);
		}
	}
	return points;
}

using portal_map_t = std::unordered_map<std::string, std::array<Point, 2U>>;


std::vector<std::string> read_lines(std::string const &filename) {
	std::ifstream infile(filename);
	if (!infile.is_open()) {
		fmt::print(stderr, "can't open file {}", filename);
	}
	std::stringstream ss { };
	ss << infile.rdbuf();
	std::vector<std::string> lines { };
	std::string line;
	while (std::getline(ss, line)) {
		lines.push_back(line);
	}
	return lines;
}

auto parse_input(std::vector<std::string> const &lines) {
	int const y_max = lines.size();
	int const x_max = lines.front().size();
	std::unordered_map<Point, std::string> point_map { };
	portal_map_t portal_map { };
	for (auto y { 1 }; y < y_max - 1; y++) {
		for (auto x { 1 }; x < x_max - 1; x++) {
			if (lines[y][x] == '.') {
				point_map[Point { x, y }] = ".";
			} else if (std::isalpha(lines[y][x])) {
				std::string portal { };
				if (lines[y][x - 1] == '.') {
					portal = fmt::format("{}{}", lines[y][x], lines[y][x + 1]);
				} else if (lines[y][x + 1] == '.') {
					portal = fmt::format("{}{}", lines[y][x - 1], lines[y][x]);
				} else if (lines[y - 1][x] == '.') {
					portal = fmt::format("{}{}", lines[y][x], lines[y + 1][x]);
				} else if (lines[y + 1][x] == '.') {
					portal = fmt::format("{}{}", lines[y - 1][x], lines[y][x]);
				}
				if (not portal.empty()) {
					point_map[Point { x, y }] = portal;
					if (portal_map.find(portal) != portal_map.end()) {
						portal_map[portal][1] = Point { x, y };
					} else {
						portal_map[portal][0] = Point { x, y };
					}
				}
			}
		}
	}
	return std::make_tuple(point_map, portal_map, Point { x_max, y_max });
}


bool find_paths(Point const &p, std::unordered_set<Point> visited, int const path_length, std::unordered_map<Point, std::string> const &point_map,
		portal_map_t const &portal_map, int &best_solution) {
	//fmt::print("{} {}\n",p, path_length);
	auto it = point_map.find(p);
	if (it != point_map.end() and it->second == "ZZ") {
		if (path_length < best_solution) {
			best_solution = path_length;
		}
		return true;
	}
	if (path_length > best_solution) {
		return false;
	}
	visited.insert(p);
	auto const neighbors = get_neighbors(p, point_map);
	for (auto n : neighbors) {
		if (visited.find(n) == visited.end()) {
			auto n_it = point_map.find(n);
			//fmt::print("neighbor: {} {}\n",n_it->first, n_it->second);
			if (n_it->second != "." and n_it->second != "ZZ") {
				//fmt::print("{} {}\n",p, path_length);
				auto portal_points = portal_map.at(n_it->second);
				if (n == portal_points[0]) {
					n = portal_points[1];
				} else {
					n = portal_points[0];
				}
			}
			auto new_len = path_length;
			if (it->second == ".") {
				new_len++;
			}
			auto res = find_paths(n, visited, new_len, point_map, portal_map, best_solution);
			if (res) {
				return res;
			}
		}
	}
	return false;
}


bool is_outer(Point &p, Point const &max_point) {
	return p.x == 1 or p.y == 1 or p.x == max_point.x - 2 or p.y == max_point.y - 2;

}


struct element {
	Point p;
	int path_length;
	int level;

};

int find_paths_part2_iter(std::unordered_map<Point, std::string> const &point_map, portal_map_t const &portal_map, Point const &max_point) {
	auto best_solution { INT32_MAX };
	std::deque<element> deq { };
	deq.emplace_back( element{ portal_map.at("AA")[0], 0, 0 });
	std::unordered_map<int, std::unordered_set<Point>> visited;
	while (not deq.empty()) {
		auto el = deq.front();
		deq.pop_front();
		auto it = point_map.find(el.p);
		if (it != point_map.end() and it->second == "ZZ" and el.level == 0) {
			if (el.path_length < best_solution) {
				best_solution = el.path_length;
			}
		}
		if (el.path_length > best_solution) {
			continue;
		}
		visited[el.level].insert(el.p);
		auto const neighbors = get_neighbors(el.p, point_map);
		for (auto n : neighbors) {
			if (visited[el.level].find(n) == visited[el.level].end()) {
				auto n_it = point_map.find(n);
				auto new_len = el.path_length;
				auto new_level = el.level;
				// IS PORTAL
				if (n_it->second != ".") {
					if (is_outer(n, max_point)) {
						if ((el.level == 0 and n_it->second != "ZZ") or (el.level != 0 and n_it->second == "ZZ")
								or (el.level != 0 and n_it->second == "AA")) {
							continue;
						} else {
							if (n_it->second != "ZZ")
								new_level--;
						}
					} else {
						new_level++;
					}
					auto portal_points = portal_map.at(n_it->second);
					if (n == portal_points[0] and n_it->second != "ZZ") {
						n = portal_points[1];
					} else {
						n = portal_points[0];
					}
				} else {
					new_len++;
				}
				auto new_element = element { n, new_len, new_level };
				deq.push_back(new_element);
			}
		}
	}
	return best_solution;

}

int main() {
	constexpr auto filename { "build/input/input_20.txt" };
	auto lines = read_lines(filename);
	auto [point_map, portal_map, max_point] = parse_input(lines);
	auto best_solution { INT32_MAX };
	find_paths(portal_map["AA"][0], std::unordered_set<Point> { }, 0, point_map, portal_map, best_solution);
	fmt::print("Part 1: {}\n", best_solution - 1);
	best_solution = find_paths_part2_iter(point_map, portal_map, max_point);
	fmt::print("Part 2: {}\n", best_solution - 1);
	return EXIT_SUCCESS;
}

