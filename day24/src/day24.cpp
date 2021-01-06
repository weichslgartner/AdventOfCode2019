#include <algorithm>
#include <array>

#include <cassert>
#include <cctype>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <tuple>
#include <fstream>
#include <functional>
#include <deque>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <unordered_set>
#include <string_view>
struct Point {
	int x;
	int y;

//	Point(int x_, int y_) :			x { x_ }, y { y_ } {	}
	bool operator==(const Point &obj) const {
		return obj.x == x && obj.y == y;
	}
	int manhattan_distance(Point const &p) const {
		return abs(x - p.x) + abs(y - p.y);
	}
};


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

int cnt_active_neighbors(Point &&p,std::vector<std::string> &grid){
	auto max_y = grid.size();
	auto max_x = grid.front().size();
	auto cnt{0};
	for(Point n :  { Point { p.x, p.y - 1 }, Point { p.x, p.y + 1 }, Point { p.x - 1, p.y }, Point { p.x + 1, p.y } }) {
		if(n.x >= 0 and n.y >= 0 and n.x <max_x and n.y<max_y ){
			if(grid[n.y][n.x] == '#'){
				cnt++;
			}
		}
	}
	return cnt;

}
unsigned calc_biodiversity(std::string_view grid_string){
	auto biodiversity {0U};
	size_t pos = grid_string.find('#');
	while(pos  != std::string::npos){
		biodiversity += 1 << pos;
		pos = grid_string.find('#',pos+1);
	}
	return biodiversity;
}

unsigned play_game(std::vector<std::string> &grid){
	std::unordered_set<std::string> seen{};
	auto new_grid = grid;
	while(true){
		auto y{0};
		auto grid_string = fmt::format("{}", fmt::join(grid,""));
		fmt::print("{}\n\n", fmt::join(grid,"\n"));
		for(auto line : grid){

			if(seen.find(grid_string) != seen.end()){
				return calc_biodiversity(grid_string);
			}
			auto x{0};
			for(auto c : line){
				auto n_active = cnt_active_neighbors(Point{x,y},grid);
				if(grid[y][x] == '#' and n_active != 1){
					new_grid[y][x] = '.';
				}else if(grid[y][x] == '.' and (n_active == 1 or n_active == 2) ){
					new_grid[y][x] = '#';
				}else{
					new_grid[y][x] = grid[y][x];
				}
				x++;
			}
			y++;
		}
		seen.insert(grid_string);
		std::swap(new_grid,grid);
	}

}


int main() {
	constexpr auto filename { "build/input/input_24.txt" };
	auto lines = read_lines(filename);
	//auto res = play_game(10,lines);
	auto res = play_game( lines);
	fmt::print("Part 1: {}\n", res); //to high 5702
	fmt::print("Part 2: {}\n", 0);
	return EXIT_SUCCESS;
}

