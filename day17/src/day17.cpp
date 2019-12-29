#include <array>
#include <assert.h>
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

enum class Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

std::string dir_to_string(Direction const dir) {
  switch (dir) {
  case Direction::RIGHT:
    return "R";
    break;
  case Direction::LEFT:
    return "L";
  case Direction::UP:
    return "U";
  case Direction::DOWN:
    return "D";
  default:
    std::cerr << "Unkown direction";
    break;
  }
  return "";
}

std::ostream &operator<<(std::ostream &out, Direction const dir) {
  {
    out << dir_to_string(dir);
    return out;
  }
}
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
        // assert(inputs.size() > 0);
        if (inputs.size() == 0) {
          char a{};
          std::cin >> a;
          std::cout << static_cast<int>(a);
          inputs.push_back(a);
        } else {
          //  std::cout << inputs.front();
        }
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

bool is_scaffold(const std::unordered_map<Point, char> &point_map,
                 const Point &p) {
  auto found = point_map.find(p);
  if (found != point_map.end() && found->second == '#') {
    return true;
  } else {
    return false;
  }
}

int count_neighbors(const std::unordered_map<Point, char> &point_map,
                    const Point &p) {
  int n_scaffolds{0};
  if (is_scaffold(point_map, Point{p.x + 1, p.y})) {
    n_scaffolds++;
  }
  if (is_scaffold(point_map, Point{p.x - 1, p.y})) {
    n_scaffolds++;
  }
  if (is_scaffold(point_map, Point{p.x, p.y + 1})) {
    n_scaffolds++;
  }
  if (is_scaffold(point_map, Point{p.x, p.y - 1})) {
    n_scaffolds++;
  }
  return n_scaffolds;
}

Point dir_to_point(const Point &p1, Direction dir) {
  switch (dir) {
  case Direction::UP:
    return Point{p1.x, p1.y - 1};
  case Direction::RIGHT:
    return Point{p1.x + 1, p1.y};
  case Direction::DOWN:
    return Point{p1.x, p1.y + 1};
  case Direction::LEFT:
    return Point{p1.x - 1, p1.y};
  default:
    break;
  }
  std::cerr << "Unkown direction " << to_type(dir) << "\n";
  return Point{-1, -1};
}

Direction turn(Direction curDir, Direction turn_dir) {
  assert(turn_dir == Direction::RIGHT || turn_dir == Direction::LEFT);
  int tmpdir{};
  if (turn_dir == Direction::RIGHT) {
    tmpdir = to_type(curDir) + 1;
    if (tmpdir > 3)
      tmpdir = 0;
  } else {
    tmpdir = to_type(curDir) - 1;
    if (tmpdir < 0)
      tmpdir = 3;
  }
  return static_cast<Direction>(tmpdir);
}

std::vector<int> find_occurences(std::string input, std::string search) {
  std::vector<int> occ{};
  auto pos = input.find(search, 0);
  while (pos != std::string::npos) {
    occ.push_back(pos);
    pos = input.find(search, pos + search.size());
  }
  return occ;
}

std::string encode_path(Point &cur_point, Direction &dir,
                        std::unordered_map<Point, char> point_map) {
  std::string result = "";
  int cur_len{1};
  while (true) {
    auto next = dir_to_point(cur_point, dir);
    if (point_map.find(next) == point_map.end()) {
      auto t = Direction::RIGHT;
      auto next_dir = turn(dir, t);
      next = dir_to_point(cur_point, next_dir);
      if (point_map.find(next) == point_map.end()) {
        t = Direction::LEFT;
        next_dir = turn(dir, t);
        next = dir_to_point(cur_point, next_dir);
      }
      dir = next_dir;
      // end of path
      if (point_map.find(next) == point_map.end()) {
        result += fmt::format("{}", cur_len);
        // result += buf.data();
        break;
      } else if (cur_len == 1) {
        result += fmt::format("{},", dir_to_string(t));
        // result += buf.data();
      } else {
        result += fmt::format("{},{},", cur_len, dir_to_string(t));
        // result += buf.data();
        cur_len = 1;
      }
    } else {
      cur_len++;
    }
    cur_point = next;
  }
  return result;
}

std::string get_program(const std::string &input) {
  std::unordered_map<std::string, char> function2name_map{};
  std::map<char, std::string> name2function_map{};
  auto cur_key = 'A';
  auto max_len = 20;
  auto begin = 0;
  auto end = 0;
  std::vector<std::pair<int, int>> prog_pos{};
  std::vector<int> found_occs;
  std::vector<char> prog;
  while (true) {
    std::string biggest_sub = "";
    /*
    for (auto pos : prog_pos) {
      if (begin >= pos.first && begin < pos.second) {
        begin = pos.second + 1;
        auto key = function2name_map.find(
            input.substr(pos.first, pos.second - pos.first));
        if (key == function2name_map.end()) {
          std::cerr
              << "subprogram not found; Something is really broken here\n";
        }
        prog.push_back(key->second);
      }
    }*/
    if (begin >= input.size()) {
      break;
    }
    if (begin > 150) {
      std::cout << "";
    }
    for (int i = 1; i < max_len;) {
      // auto c = input.at(begin + i - 1);
      if (input.at(begin + i - 1) == 'L' || input.at(begin + i - 1) == 'R') {
        i += 2;
      }

      ///  c = input.at(begin + i - 1);
      if (input.at(begin + i - 1) == ',') {
        i++;
        continue;
      }

      if (begin + i > input.size()) {
        break;
      }
      auto sub = input.substr(begin, i);
      auto occs = find_occurences(input, sub);
      if (function2name_map.find(sub) != function2name_map.end()) {
        found_occs.clear();
        found_occs.insert(found_occs.end(), occs.begin(), occs.end());
        biggest_sub = sub;
        end = begin + i;
        break;
      } else if (occs.size() >= 2) {
        found_occs.clear();
        found_occs.insert(found_occs.end(), occs.begin(), occs.end());
        biggest_sub = sub;
        end = begin + i;
      } else {
        break;
      }

      i += 2;
      if (begin + i > input.size()) {
        break;
      }
    }
    for (int o : found_occs) {
      auto temp = std::make_pair(o, o + end - begin);
      prog_pos.push_back(temp);
    }
    begin = end + 1;
    auto found = function2name_map.find(biggest_sub);
    if (found == function2name_map.end()) {
      function2name_map.insert({biggest_sub, cur_key});
      name2function_map.insert({cur_key, biggest_sub});
      prog.push_back(cur_key);
      cur_key++;
    } else {
      prog.push_back(found->second);
    }
  }
  std::string result = "";
  for (auto p : prog) {
    result += p;
    result += ",";
  }
  result = result.substr(0, result.size() - 1); // cut last comma
  result += "\n";

  for (auto el : name2function_map) {
    result += el.second;
    result += "\n";
  }
  return result;
}

Point parse_point_map_file(Point cur_point,
                           std::unordered_map<Point, char> &point_map,
                           Direction &dir) {
  std::ifstream infile("./input/input_17_test.txt");
  std::stringstream ss{};
  ss << infile.rdbuf();
  auto string = ss.str();
  char a{};
  int x{0};
  int y{0};
  for (char const &a : string) {
    switch (a) {
    case '#':
      point_map.insert({Point{x, y}, a});
      x++;
      break;
    case '.':
      x++;
      break;
    case '^':
      cur_point = Point{x, y};
      dir = Direction::UP;
      x++;
      break;
    case '\n':
      y++;
      x = 0;
      break;
    default:
      break;
    }
    std::cout << a;
  }
  std::cout << "\n";
  return cur_point;
}

void part2() {
  std::unordered_map<Point, char> point_map;
  Point cur_point{};
  Direction dir{};
  cur_point = parse_point_map_file(cur_point, point_map, dir);
  std::string result = encode_path(cur_point, dir, point_map);
  auto program = get_program(result);
  std::cout << program;
}

int get_picture(std::vector<long long int> &vec) {
  Interpreter inter{vec};
  std::deque<long long int> inputs{};
  std::unordered_map<Point, char> point_map;
  char output{};
  bool halted{false};
  int x{0};
  int y{0};
  Point cur_point{};
  Direction dir{};
  while (!halted) {
    std::tie(output, halted) = inter.run_programm(inputs);
    switch (static_cast<char>(output)) {
    case '#':
      point_map.insert({Point{x, y}, output});
      x++;
      break;
    case '.':
      x++;
      break;
    case '^':
      cur_point = Point{x, y};
      dir = Direction::UP;
      x++;
      break;
    case '\n':
      y++;
      x = 0;
      break;
    case ':':
      halted = true;
      break;
    default:
      break;
    }
    std::cout << output;
  }
  int aligment{0};
  for (const auto &key_val : point_map) {
    auto point = key_val.first;
    if (count_neighbors(point_map, point) == 4) {
      aligment += point.x * point.y;
    }
  }

  std::string result = encode_path(cur_point, dir, point_map);
  // std::cout << result << std::endl;
  auto program = get_program(result);
  std::cout << program << std::endl;
  for (auto const c : program) {
    //  inputs.push_back(c);
  }
  halted = false;
  while (!halted) {
    std::tie(output, halted) = inter.run_programm(inputs);
    std::cout << output << " ";
  }
  return aligment;
}

int main() {
  // part2();

  tests();
  std::ifstream infile("./input/input_17.txt");
  std::vector<long long int> vec{};
  std::stringstream ss{};
  ss << infile.rdbuf();
  vec = string2vector(ss);
  vec.resize(10000, 0LL);
  auto vec2 = vec;

  //  auto aligment = get_picture(vec);

  // std::cout << "Part 1: " << aligment << "\n";
  vec2[0] = 2;
  get_picture(vec2);
  std::cout << "Part 2: "
            << ""
            << "\n";
  return 0;
}
