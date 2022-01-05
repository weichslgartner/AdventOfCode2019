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
#include <numeric>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <set>
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
		std::size_t val = 100000 * p.y + p.x;
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
	std::string locks;
	std::string keys;
};

std::ostream& operator<<(std::ostream &out, PointCost const &p) {
	out << p.point << ":" << p.cost << " "  << p.locks << " "<<  p.keys <<";";
	return out;
}

struct ExploreElement {
	std::set<char> keys;
	std::set<char> locks;
	std::array<Point,4> cur_point;
	int steps;

	bool operator==(const ExploreElement &other) const {
		return  std::string( keys.begin(), keys.end()) ==  std::string( other.keys.begin(), other.keys.end()) &&
				//std::string( locks.begin(), locks.end()) ==  std::string( other.locks.begin(), other.locks.end());
				cur_point[0] == other.cur_point[0] &&
				cur_point[1] == other.cur_point[1] &&
				cur_point[2] == other.cur_point[2] &&
				cur_point[3] == other.cur_point[3];
	}
};

std::ostream& operator<<(std::ostream &out, ExploreElement const &ee) {
	out << "keys " << std::string( ee.keys.begin(), ee.keys.end()) << " locks " << std::string( ee.locks.begin(), ee.locks.end()) << " " << ee.cur_point[0] 
	<< " " << ee.cur_point[1] << " " << ee.cur_point[2] << " " << ee.cur_point[3];
	return out;
}


namespace std {
template<>
struct hash<ExploreElement> {
	std::size_t operator()(const ExploreElement &e) const {
		std::string keys = std::string( e.keys.begin(), e.keys.end());
		return std::hash<std::string>{}(keys) 
				^ std::hash<Point>{}(e.cur_point[0])
				^ std::hash<Point>{}(e.cur_point[1])
				^ std::hash<Point>{}(e.cur_point[2])
				^ std::hash<Point>{}(e.cur_point[3]) ;  
	}
};
}


struct LessThanExploreElement {
	bool operator()(const ExploreElement &lhs, const ExploreElement &rhs) const {
		return lhs.keys.size()+ lhs.steps  >  rhs.keys.size()+ rhs.steps;
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
		const std::unordered_map<Point, std::vector<PointCost>> &reach_map, const Point &point,
		const std::set<char> &keys, const std::set<char> &locks, std::vector<PointCost> &result) {
	auto next_moves = reach_map.find(point)->second;

	for(auto &move : next_moves) {
		auto [dst_point, dst_costs, needed_locks, passed_keys] = move;
		auto dst_type = point_map.find(dst_point)->second;
		bool have_all_keys = true;
		for(auto c :needed_locks){
			if(keys.find(std::tolower(c)) == keys.end()) {
				have_all_keys = false;
				break;
			}
		}
		if(!have_all_keys){
			continue;
		}
		bool got_already = true;
		for(auto c : passed_keys){
			if(keys.find(c) == keys.end() && c!= dst_type) {
				got_already = false;
				break;
			}
		}
		if(!got_already){
			continue;
		}
		if(is_key(dst_type) && keys.find(dst_type) != keys.end()) {
			continue;
		}
		if (is_lock(dst_type)) {
			if(locks.find(dst_type) != locks.end()){
				continue;
			}
			// found a  new lock where we have the key
			auto need_key = std::tolower(dst_type);
			if (keys.find(need_key) != keys.end()) {
				result.push_back(PointCost { dst_point, dst_costs,needed_locks });
			}
			
		}

		result.push_back(PointCost { dst_point, dst_costs,needed_locks });
		
		
	}
	return result;
}

std::vector<PointCost> get_next_moves(const std::unordered_map<Point, char> &point_map, const Point &start_point, const std::set<char> keys,
		const std::set<char> locks) {
	std::deque<PointCost> deq;
	std::vector<PointCost> result;


	std::unordered_set<Point> visited { };
	
	std::unordered_map<Point, int> cost_map { };

	deq.push_back( { start_point,  0,""});
	visited.insert(start_point);
	cost_map.insert( { start_point, 0 });
	if(start_point.x ==5 && start_point.y == 2 ){
		std::cout <<"gebug";
	}
	while (!deq.empty()) {
		auto el = deq.front();
		deq.pop_front();
		auto [cur_point, cost, needed_locks, passed_keys] = el;
		auto found = point_map.find(cur_point);
		if((is_lock(found->second) ||is_key(found->second))  && cost != 0){
			result.push_back(el);
		}
		auto neighbors = get_avail_neighbors(point_map, cur_point);
		for (auto neighbor : neighbors) {
			auto found = point_map.find(neighbor);
			assert(found != point_map.end());
			auto new_needed_locks = needed_locks;
			auto new_costs = update_costs(cost_map, neighbor, cost);
 			if (is_lock(found->second) && needed_locks.find(found->second) ==std::string::npos) {
				new_needed_locks += found->second;
			}
			auto new_passed_keys = passed_keys;
 			if (is_key(found->second) && passed_keys.find(found->second) ==std::string::npos) {
				new_passed_keys += found->second;
			}


			if (visited.find(neighbor) == visited.end()) {
				
				deq.push_back( { neighbor, new_costs, new_needed_locks, new_passed_keys});
			}
			visited.insert(neighbor);
		}
	}
	return result;
}





int find_min_steps(const std::unordered_map<Point, char> &point_map, std::set<char> &keys_to_find,
		const std::unordered_map<Point, std::vector<PointCost>> &reach_map, const std::unordered_map<char, Point> &POI_map, std::vector<Point> &start) {
	std::priority_queue<ExploreElement, std::vector<ExploreElement>, LessThanExploreElement> deq { };
	std::set<char> init_keys;
	//init_keys.reserve(27);
	std::set<char> init_locks;
	//init_locks.reserve(27);
	ExploreElement el { init_keys, init_locks, std::array<Point,4>{start[0],start[1],start[2],start[3]}, 0 };
	deq.push(el);
	std::vector<PointCost> next_moves;
	std::set<char> visited;
	std::unordered_map<ExploreElement,int> visited_elements{};
	int min_steps { 4280 };  // 4844 too high  6768 too high INT32_MAX wrong 4280
	while (!deq.empty()) {
		auto el = deq.top();
		deq.pop();

		
		auto [keys, locks, cur_point, steps] = el;
		std::cout << cur_point[0] << cur_point[1]<<cur_point[2]<<cur_point[3] << " " << point_map.find(cur_point[0])->second << " " << std::string(keys.begin(),keys.end()) <<" " << steps << '\n';
		/* for(auto &ve : visited_elements){
			std::cout << ve.first << " " << ve.second << " ";
		}
		std::cout << '\n'; */
		if (keys.size() == keys_to_find.size()) {
			if (steps < min_steps) {
				min_steps = steps;
				std::cout << min_steps << " \n";
				return min_steps;
			}

		} else if (steps > min_steps) {  // 6768

			// do nothing
		} else {
			next_moves.clear();
			//   auto next_moves = get_next_moves(pmap, cur_point, keys, locks);
			for(auto &p : cur_point){
				next_moves = get_next_moves_pre(point_map, reach_map, p, keys, locks, next_moves);
			}
			//    std::cout << cur_point << " :";
			for (auto [next_point, next_cost, needed_locks, passed_keys] : next_moves) {
				//    std::cout << next_point << ";";

				// std::unordered_map<Point, char> new_pmap = pmap;
				auto found = point_map.find(next_point);
				assert(found != point_map.end());
				std::set<char> new_keys = keys;
				std::set<char> new_locks = locks;
				if (is_key(found->second)) {
					new_keys.insert(found->second);
				}
				if (is_lock(found->second)) {
					new_locks.insert(found->second);
				}
				// new_pmap[next_point] = '.';
				auto next_steps = steps + next_cost;
				ExploreElement next_el { new_keys, new_locks, cur_point, next_steps };
				if(next_point.x <= start[0].x && next_point.y <= start[0].y){
					next_el.cur_point[0]=next_point;
				}else if(next_point.x >= start[1].x && next_point.y <= start[1].y){
					next_el.cur_point[1]=next_point;
				}else if(next_point.x <= start[2].x && next_point.y >= start[2].y){
					next_el.cur_point[2]=next_point;
				}else if(next_point.x >= start[3].x && next_point.y >= start[3].y){
					next_el.cur_point[3]=next_point;
				}else{
					fmt::print("should not happen");
				}

				if (visited_elements.find(next_el) != visited_elements.end() && visited_elements[next_el] <= next_el.steps){
					continue;
				}
				if (next_el.steps > min_steps){
					continue;
				}
				deq.push(next_el);
				visited_elements[next_el] =next_el.steps;
				
				
			}
			//  std::cout << "\n";
		}
	}

	return min_steps;
}

std::vector<Point> parse_input(std::string const &filename, std::unordered_map<Point, char> &point_map, std::unordered_map<char, Point> &POI_map,
		std::set<char> &all_keys, std::set<char> &all_locks) {
	std::ifstream infile(filename);
	if (not infile.is_open()) {
		std::cerr << "can't open file " << filename;
	}
	std::stringstream ss { };
	std::vector<Point> start_points { };
	ss << infile.rdbuf();
	auto string = ss.str();
	int x { 0 };
	int y { 0 };
	char startpoint_dec = 0;
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
			POI_map.insert( { a-startpoint_dec, Point { x, y } });
			point_map.insert( { Point { x, y }, '.' });
			start_points.push_back(Point { x, y });
			startpoint_dec++;
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
	return start_points;
}

void print_reach_map(std::unordered_map<Point, std::vector<PointCost>> const &reach_map ,std::unordered_map<Point, char> &point_map ){
	for (auto [origin, dsts] : reach_map) {
		std::cout << origin << " " << point_map[origin] << ": ";
		for (auto const &dst : dsts) {
			std::cout << point_map[dst.point] << " " << dst << " ";
		}
		std::cout << "\n";
	}
}

std::unordered_map<Point, std::vector<PointCost>>   create_reach_map(const std::unordered_map<Point, char> &point_map,
		std::unordered_map<char, Point> &POI_map) {
	std::set<char> keys { };
	std::set<char> locks { };
	std::unordered_map<Point, std::vector<PointCost>> reach_map { };
	for (auto [src_key, src_point] : POI_map) {
		reach_map.insert( { src_point, std::vector<PointCost> { } });
		keys.clear();
		if (is_key(src_key)) {
			keys.insert(src_key);
		}
		std::cout << src_key << src_point << std::endl;
		auto next_moves = get_next_moves(point_map, src_point, keys, locks);
		for (auto const &next_move : next_moves) {
			reach_map.find(src_point)->second.push_back(next_move);
		}
	}
	return reach_map;
}

#ifndef TESTING
int main() {
	std::string base = "/home/andreas/githubprojects/AdventOfCode2019/";
	auto filename {base +  "./build/input/input_18_part2.txt" };
	std::unordered_map<Point, char> point_map;
	std::set<char> keys_to_find;
	std::set<char> all_locks;
	std::unordered_map<char, Point> POI_map;
	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
	auto reach_map = create_reach_map(point_map, POI_map);
	for (auto [origin, dsts] : reach_map) {
		std::cout << origin << " " << point_map[origin] << ":";
		for (auto dst : dsts) {
			std::cout << point_map[dst.point] << " " << dst << " ";
		}
		std::cout << "\n";
	}

	auto part2 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);


	std::cout << "Part 2: " << part2<< '\n';
	return 0;
}
#endif
