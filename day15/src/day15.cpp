#include <array>
#include <assert.h>
#include <deque>
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

struct Point {
public:
  int x;
  int y;

  const bool operator==(const Point &obj) const {
    return obj.x == x && obj.y == y;
  }
  int manhattan_distance(Point p) { return abs(x - p.x) + abs(y - p.y); }
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

enum class State { UNKOWN = 0, FREE = 1, WALL = 2, DEADEND = 3, OXYGEN = 4 };

enum class Direction { NORTH = 1, SOUTH = 2, WEST = 3, EAST = 4 };

enum class Droid_Out { WALL = 0, MOVED = 1, OXYGEN = 2 };

enum class Opcode {
  ADD = 1,
  MULT = 2,
  INPUT = 3,
  OUTPUT = 4,
  JIT = 5,
  JIF = 6,
  LT = 7,
  EQ = 8,
  SET_RB = 9,
  HALT = 99
};

enum class Mode { POSITION = 0, IMMEDIATE = 1, RELATIVE = 2 };

struct Instruction {
  Opcode opcode;
  Mode arg1;
  Mode arg2;
  Mode arg3;

public:
  Instruction(Opcode op, Mode a1 = Mode::POSITION, Mode a2 = Mode::POSITION,
              Mode a3 = Mode::POSITION) {
    opcode = op;
    arg1 = a1;
    arg2 = a2;
    arg3 = a3;
  }
};

Instruction parse_instruction(int inst) {
  std::string str_inst = std::to_string(inst);
  str_inst = std::string(5 - str_inst.length(), '0') + str_inst;
  auto op = static_cast<Opcode>(std::stoi(str_inst.substr(3, 2)));
  auto a1 = static_cast<Mode>(std::stoi(str_inst.substr(2, 1)));
  auto a2 = static_cast<Mode>(std::stoi(str_inst.substr(1, 1)));
  auto a3 = static_cast<Mode>(std::stoi(str_inst.substr(0, 1)));
  return Instruction(op, a1, a2, a3);
}

template <typename T> void print_vector(std::vector<T> vec) {
  for (auto v : vec) {
    std::cout << v << " ";
  }
  std::cout << "\n";
}

class Interpreter {
public:
  int ins_pointer = 0;
  long long int relative_base = 0;
  std::vector<long long int> outputs;
  std::vector<long long int> program;
  bool return_on_output; // already return if there is an output, not wait for
                         // HALT
  long long int extract_one_rh(Mode mode, long long position) {
    long long int abs_position{};
    check_resize(ins_pointer + position);
    if (mode == Mode::POSITION) {
      abs_position = program[ins_pointer + position];
    } else if (mode == Mode::RELATIVE) {
      abs_position = relative_base + program[ins_pointer + position];
    } else {
      abs_position = ins_pointer + position;
    }
    check_resize(abs_position);

    auto arg1 = program[abs_position];
    if (VERBOSE)
      std::cout << arg1 << " ";
    return arg1;
  }

  std::pair<long long int, long long int>
  extract_two_rh(Instruction const &inst) {
    auto arg1 = extract_one_rh(inst.arg1, 1);
    auto arg2 = extract_one_rh(inst.arg2, 2);
    return std::make_pair(arg1, arg2);
  }

  void assing(const Mode mode, unsigned position, long long int rh) {
    long long int abs_position{};
    check_resize(ins_pointer + position);
    if (mode == Mode::POSITION) {
      abs_position = program[ins_pointer + position];

    } else if (mode == Mode::RELATIVE) {
      abs_position = relative_base + program[ins_pointer + position];
    } else {
      abs_position = ins_pointer + position;
    }
    check_resize(abs_position);
    program[abs_position] = rh;
    if (VERBOSE)
      std::cout << abs_position << " ";
  }

public:
  Interpreter() { return_on_output = true; }
  Interpreter(std::vector<long long int> vec, bool rop = true) {
    program = vec;
    return_on_output = rop;
  }
  // returns output and if program is halted
  std::pair<long long int, bool>
  run_programm(std::deque<long long int> &inputs) {
    long long int temp{0};
    long long int arg1{}, arg2{}, arg3{};
    while (true) {
      Instruction inst = parse_instruction(program[ins_pointer]);
      if (VERBOSE) {
        std::cout << "\n";
        std::cout << to_type(inst.opcode) << " " << ins_pointer << " "
                  << relative_base << " ";
      }
      switch (inst.opcode) {
      case (Opcode::ADD):
        std::tie(arg1, arg2) = extract_two_rh(inst);
        temp = arg1 + arg2;
        assing(inst.arg3, 3, temp);
        ins_pointer += 4;
        break;
      case (Opcode::MULT):
        std::tie(arg1, arg2) = extract_two_rh(inst);
        temp = arg1 * arg2;
        assing(inst.arg3, 3, temp);
        ins_pointer += 4;
        break;
      case Opcode::INPUT:
        assert(inputs.size() > 0);
        temp = inputs.front();
        assing(inst.arg1, 1, temp);
        inputs.pop_front();
        ins_pointer += 2;
        break;
      case Opcode::OUTPUT:
        arg1 = extract_one_rh(inst.arg1, 1);
        outputs.push_back(arg1);
        ins_pointer += 2;
        if (return_on_output)
          return std::make_pair(arg1, false);
        break;
      case Opcode::JIT:
        std::tie(arg1, arg2) = extract_two_rh(inst);
        if (arg1 != 0) {
          ins_pointer = arg2;
        } else {
          ins_pointer += 3;
        }
        break;
      case Opcode::JIF:
        std::tie(arg1, arg2) = extract_two_rh(inst);
        if (arg1 == 0) {
          ins_pointer = arg2;
        } else {
          ins_pointer += 3;
        }
        break;
      case Opcode::LT:
        std::tie(arg1, arg2) = extract_two_rh(inst);
        if (arg1 < arg2) {
          assing(inst.arg3, 3, 1);
        } else {
          assing(inst.arg3, 3, 0);
        }
        ins_pointer += 4;
        break;
      case Opcode::EQ:
        std::tie(arg1, arg2) = extract_two_rh(inst);
        if (arg1 == arg2) {
          assing(inst.arg3, 3, 1);
        } else {
          assing(inst.arg3, 3, 0);
        }
        ins_pointer += 4;
        break;
      case Opcode::SET_RB:
        relative_base += extract_one_rh(inst.arg1, 1);
        ins_pointer += 2;
        break;
      case Opcode::HALT:
        return std::make_pair(outputs[outputs.size() - 1], true);
      default:
        std::cout << "Unkown Opcode " << program[ins_pointer] << "\n";
        break;
      }
    }
    // never reached
    return std::make_pair(-1, false);
  }

private:
  void check_resize(long long int abs_position) {
    if (abs_position >= program.size()) {
      program.resize(abs_position + 1, 0LL);
    }
  }
};

std::vector<long long int> string2vector(std::stringstream &ss) {
  std::vector<long long int> vec{};
  long long int a{};
  while (ss >> a) {
    vec.push_back(a);
    if (ss.peek() == ',')
      ss.ignore();
  }
  return vec;
}

void tests() {
  std::vector<long long int> vec{};
  std::stringstream ss{};
  long long int output{};
  bool halted{};
  ss << "1102,34915192,34915192,7,4,7,99,0";
  vec = string2vector(ss);
  Interpreter interpreter{vec, false};
  std::deque<long long int> inputs{1};
  std::tie(output, halted) = interpreter.run_programm(inputs);
  assert(output == 1219070632396864);
  ss = std::stringstream();
  ss << "104,1125899906842624,99";
  vec = string2vector(ss);
  interpreter = Interpreter{vec, true};
  std::tie(output, halted) = interpreter.run_programm(inputs);
  assert(output == 1125899906842624);
  ss = std::stringstream();
  std::string prog3 =
      "109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99";
  ss << prog3;
  vec = string2vector(ss);
  interpreter = Interpreter{vec, true};
  std::string outstring = "";
  while (!halted) {
    std::tie(output, halted) = interpreter.run_programm(inputs);
    outstring += std::to_string(output) + ",";
  }
  outstring = outstring.substr(0, prog3.size());
  assert(outstring.compare(prog3) == 0);
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

int calc_costs(int cur_costs, Direction dir,
               std::unordered_map<Point, int> &cost_map, const Point curpoint) {
  cur_costs = cost_map[curpoint];
  auto nextpoint = dir_to_point(curpoint, dir);
  auto found = cost_map.find(nextpoint);
  if (found == cost_map.end()) {
    cost_map.insert({nextpoint, cur_costs + 1});
  } else {
    if ((cur_costs + 1) < found->second) {
      found->second = cur_costs + 1;
    }
  }
  return cur_costs + 1;
}

std::array<State, 4> &
get_point(std::unordered_map<Point, std::array<State, 4>> &point_map,
          Point &curpoint, std::array<State, 4> &arr) {
  auto found = point_map.find(curpoint);
  if (found == point_map.end()) {
    // init with unkown neighbors
    arr.fill(State::UNKOWN);
    point_map.insert({curpoint, arr});
  } else {
    arr = found->second;
  }
  return arr;
}

bool get_next_dirs(std::array<State, 4> const &arr,
                   std::vector<Direction> &next_directions) {
  // possible steps
  bool dead_end{false};
  next_directions.clear();
  for (int i = 0; i < 4; ++i) {
    if (arr[i] == State::UNKOWN) {
      // offset by 1
      next_directions.push_back(static_cast<Direction>(i + 1));
    }
  }
  // if all explored go back
  if (next_directions.size() == 0) {
    dead_end = true;
    for (int i = 0; i < 4; ++i) {
      if (arr[i] == State::FREE) {
        // offset by 1
        next_directions.push_back(static_cast<Direction>(i + 1));
      }
    }
  }
  return dead_end;
}

int spread_oxygen(Point oxy_source,
                  std::unordered_map<Point, std::array<State, 4>> point_map) {
  std::deque<Point> deq{};
  deq.push_back(oxy_source);
  int minutes{0};
  while (!deq.empty()) {
    std::vector<Point> next_points{};
    for (auto el : deq) {
      auto arr = point_map[el];
      for (int i = 0; i < 4; ++i) {
        if ((arr[i] != State::WALL) && (arr[i] != State::OXYGEN)) {
          auto next = dir_to_point(el, static_cast<Direction>(i + 1));
          point_map.find(el)->second[i] = State::OXYGEN;
          auto dir_back = point_to_dir(next, el);
          point_map.find(next)->second[to_type(dir_back) - 1] = State::OXYGEN;
          next_points.push_back(next);
        }
      }
    }
    deq.clear();
    deq.insert(deq.end(), next_points.begin(), next_points.end());
    minutes++;
  }
  return minutes - 1;
}

std::pair<Point, int>
find_oxygen(std::vector<long long int> &vec,
            std::unordered_map<Point, std::array<State, 4>> &point_map,
            bool stop_oxy_found) {
  Interpreter inter{vec};
  std::vector<Direction> next_directions{};
  std::deque<long long int> inputs{};

  std::unordered_map<Point, int> cost_map;
  Point curpoint{0, 0};
  Point oxy_source{};
  std::array<State, 4> arr{State::UNKOWN, State::UNKOWN, State::UNKOWN,
                           State::UNKOWN};
  point_map.insert({curpoint, arr});
  cost_map.insert({curpoint, 0});
  int cur_costs{0};
  while (true) {
    arr = get_point(point_map, curpoint, arr);
    // possible steps
    bool dead_end = get_next_dirs(arr, next_directions);
    if (next_directions.size() == 0) {
      break;
    }
    long long result{0};
    bool halted{false};
    // try until
    for (auto dir : next_directions) {
      inputs.push_back(to_type(dir));
      std::tie(result, halted) = inter.run_programm(inputs);
      if (result == 0) {
        arr[to_type(dir) - 1] = State::WALL;
        point_map[curpoint] = arr;
      } else if (result == 1) {
        State cur_state{};
        if (dead_end)
          cur_state = State::DEADEND;
        else
          cur_state = State::FREE;
        arr[to_type(dir) - 1] = cur_state;
        cur_costs = calc_costs(cur_costs, dir, cost_map, curpoint);
        point_map[curpoint] = arr;
        auto next_point = dir_to_point(curpoint, dir);
        arr = get_point(point_map, next_point, arr);
        auto dir_to_pred = point_to_dir(next_point, curpoint);
        arr[to_type(dir_to_pred) - 1] = cur_state;
        point_map[next_point] = arr;
        curpoint = next_point;
        break;
      } else if (result == 2) {
        State cur_state{};

        cur_costs = calc_costs(cur_costs, dir, cost_map, curpoint);
        /*
        std::cout << "found " << dir_to_point(curpoint, dir)
                  << " after steps:" << cur_costs << "\n";
                  */
        cur_state = State::FREE;
        point_map[curpoint] = arr;
        auto next_point = dir_to_point(curpoint, dir);
        arr = get_point(point_map, next_point, arr);
        auto dir_to_pred = point_to_dir(next_point, curpoint);
        arr[to_type(dir_to_pred) - 1] = cur_state;
        point_map[next_point] = arr;
        curpoint = next_point;
        oxy_source = curpoint;
        if (stop_oxy_found)
          return std::make_pair(oxy_source, cost_map[oxy_source]);
      }
    }
  }
  return std::make_pair(oxy_source, cost_map[oxy_source]);
}

int main() {
  tests();
  std::ifstream infile("./input/input_15.txt");
  std::vector<long long int> vec{};
  std::stringstream ss{};
  ss << infile.rdbuf();
  vec = string2vector(ss);
  vec.resize(10000, 0LL);
  auto vec2 = vec;
  std::unordered_map<Point, std::array<State, 4>> point_map{};
  auto [oxy_source, costs] = find_oxygen(vec, point_map, false);
  std::cout << "Part 1: " << costs << "\n";
  auto minutes = spread_oxygen(oxy_source, point_map);
  std::cout << "Part 2: " << minutes << "\n";
  return 0;
}
