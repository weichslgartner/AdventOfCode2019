#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <string_view>

struct Point {
	int x;
	int y;
	bool operator==(const Point &obj) const {
		return obj.x == x && obj.y == y;
	}
	int manhattan_distance(Point const &p) const {
		return abs(x - p.x) + abs(y - p.y);
	}
};

struct Point3D {
	int x;
	int y;
	int z;
	inline int manhattan_distance(Point3D const &other) const {
		return std::abs(x - other.x) + std::abs(y - other.y) + std::abs(z - other.z);
	}
	bool operator==(const Point3D &obj) const {
		return obj.x == x && obj.y == y && obj.z == z;
	}
};

namespace fmt {
template<>
struct formatter<Point3D> {
	template<typename ParseContext>
	constexpr auto parse(ParseContext &ctx) const {
		return ctx.begin();
	}
	template<typename FormatContext>
	auto format(Point3D const &p, FormatContext &ctx) const {
		return fmt::format_to(ctx.out(), "({},{},{})", p.x, p.y,p.z);
	}
};
}

namespace std {
template<>
struct hash<Point3D> {
	std::size_t operator()(const Point3D &p) const {
		std::size_t val { 0 };
		val = (1 << 20) * p.z + (1 << 10) * p.y + p.x;
		return val;
	}
};
}

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

std::vector<std::string> read_tokenz(std::string_view const line) {
	std::stringstream ss { };
	ss << line;
	std::vector<std::string> tokenz { };
	std::string token;
	while (std::getline(ss, token, ' ')) {
		tokenz.push_back(token);
	}
	return tokenz;
}

int cnt_active_neighbors(Point &&p, std::vector<std::string> &grid) {
	auto const max_y { grid.size() };
	auto const max_x { grid.front().size() };
	auto cnt { 0 };
	for (Point const &n : { Point { p.x, p.y - 1 }, Point { p.x, p.y + 1 }, Point { p.x - 1, p.y }, Point { p.x + 1, p.y } }) {
		if (n.x >= 0 and n.y >= 0 and n.x < max_x and n.y < max_y) {
			if (grid[n.y][n.x] == '#') {
				cnt++;
			}
		}
	}
	return cnt;
}

int cnt_3D_bugs(std::vector<Point3D> &points, std::unordered_set<Point3D> const &bugs) {
	return std::count_if(points.begin(), points.end(), [&bugs](auto &p) {
		return bugs.find(p) != bugs.end();
	});

}

std::vector<Point3D> get_neighbors_3D(Point3D const &p) {
	constexpr auto const max_x{5};
	constexpr auto const max_y{5};
	std::vector<Point3D> neighbors;
	for (Point3D const &n : { Point3D { p.x, p.y - 1, p.z }, Point3D { p.x, p.y + 1, p.z }, Point3D { p.x - 1, p.y, p.z },
			Point3D { p.x + 1, p.y, p.z } }) {
		if (n.x == -1) {
			neighbors.push_back(Point3D { 1, 2, p.z + 1 });
		} else if (n.x == max_x) {
			neighbors.push_back(Point3D { 3, 2, p.z + 1 });
		} else if (n.y == -1) {
			neighbors.push_back(Point3D { 2, 1, p.z + 1 });
		} else if (n.y == max_y) {
			neighbors.push_back(Point3D { 2, 3, p.z + 1 });

		} else if (n.x == 2 and n.y == 2) {
			if (p.x == 1) {
				for (auto y { 0 }; y < max_y; ++y) {
					neighbors.push_back(Point3D { 0, y, p.z - 1 });
				}
			} else if (p.x == 3) {
				for (auto y { 0 }; y < max_y; ++y) {
					neighbors.push_back(Point3D { max_x - 1, y, p.z - 1 });
				}
			} else if (p.y == 1) {
				for (auto x { 0 }; x < max_x; ++x) {
					neighbors.push_back(Point3D { x, 0, p.z - 1 });
				}
			} else if (p.y == 3) {
				for (auto x { 0 }; x < max_x; ++x) {
					neighbors.push_back(Point3D { x, max_y - 1, p.z - 1 });
				}
			} else {
				fmt::print(stderr, "error\n");
			}

		} else {
			neighbors.push_back(n);
		}
	}
	return neighbors;

}

void fill_bugs(std::vector<std::string> const &grid, std::unordered_set<Point3D> &bugs) {
	auto y { 0 };
	for (auto const line : grid) {
		auto x { 0 };
		for (auto c : line) {
			if (c == '#') {
				bugs.emplace(Point3D { x, y, 0 });
			}
			x++;
		}
		y++;
	}
}

void print_bug_grid(std::unordered_set<Point3D> const &bugs) {
	auto [min_level, max_level] = std::minmax_element(bugs.begin(), bugs.end(), [](auto const &p1, auto const &p2) {
		return p1.z < p2.z;
	});
	fmt::print("min level: {}  max_level {}\n", min_level->z, min_level->z);
	for (auto z { min_level->z }; z <= min_level->z; ++z) {
		fmt::print("level: {}\n",z);
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				if(bugs.find(Point3D{x,y,z})!= bugs.end()){
					fmt::print("#");
				}else{
					fmt::print(".");
				}
			}
			fmt::print("\n");
		}
		fmt::print("\n");
	}

}

unsigned part2(std::vector<std::string> const &grid, int const iterations) {
	std::unordered_set<Point3D> bugs{};
	std::unordered_set<Point3D> bug_neighbors{};
	fill_bugs(grid, bugs);
	for (auto i { 0 }; i < iterations; ++i) {
		std::unordered_set<Point3D> bugs_next = bugs;
		bug_neighbors.clear();
		//print_bug_grid(bugs);
		for (auto const &bug : bugs) {
			auto neighbors = get_neighbors_3D(bug);
			//fmt::print("bug {} has neighbors: {}\n", bug, fmt::join(neighbors,","));
			auto const bug_cnt = cnt_3D_bugs(neighbors, bugs);
			std::copy_if(neighbors.begin(), neighbors.end(), std::inserter(bug_neighbors, bug_neighbors.end()), [&bugs](auto const &bug) {
				return bugs.find(bug) == bugs.end();
			});
			if (bug_cnt != 1) {
				bugs_next.erase(bug);
			}
		}
		for (auto const &bug : bug_neighbors) {
			auto neighbors = get_neighbors_3D(bug);
			auto const bug_cnt = cnt_3D_bugs(neighbors, bugs);
			if (bug_cnt == 1 or bug_cnt == 2) {
				bugs_next.insert(bug);
			}
		}
		std::swap(bugs, bugs_next);
	}
	return bugs.size();
}

unsigned calc_biodiversity(std::string_view const grid_string) {
	auto biodiversity { 0U };
	size_t pos = grid_string.find('#');
	while (pos != std::string::npos) {
		biodiversity += 1 << pos;
		pos = grid_string.find('#', pos + 1);
	}
	return biodiversity;
}

unsigned play_game(std::vector<std::string> grid) {
	std::unordered_set<std::string> seen { };
	auto new_grid = grid;
	while (true) {
		auto y { 0 };
		auto grid_string = fmt::format("{}", fmt::join(grid, ""));
		//fmt::print("{}\n\n", fmt::join(grid, "\n"));
		for (auto const &line : grid) {
			if (seen.find(grid_string) != seen.end()) {
				return calc_biodiversity(grid_string);
			}
			auto x { 0 };
			for (auto c : line) {
				auto n_active = cnt_active_neighbors(Point { x, y }, grid);
				if (grid[y][x] == '#' and n_active != 1) {
					new_grid[y][x] = '.';
				} else if (grid[y][x] == '.' and (n_active == 1 or n_active == 2)) {
					new_grid[y][x] = '#';
				} else {
					new_grid[y][x] = grid[y][x];
				}
				x++;
			}
			y++;
		}
		seen.insert(grid_string);
		std::swap(new_grid, grid);
	}

}

int main() {
	constexpr auto filename { "build/input/input_24.txt" };
	auto lines = read_lines(filename);
	//auto res = play_game(10,lines);
	auto res = play_game(lines);
	fmt::print("Part 1: {}\n", res); //to high 5702
	fmt::print("Part 2: {}\n", part2(lines, 200));
	return EXIT_SUCCESS;
}

