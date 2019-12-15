#include <assert.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template <typename T> constexpr auto to_type(T e) {
  return static_cast<typename std::underlying_type<T>::type>(e);
}
enum class Color { BLACK = 0, WHITE = 1 };
enum class Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };
constexpr Direction left_dir(Direction dir) {
  auto val = to_type<Direction>(dir);
  val--;
  if (val < 0) {
    val = 3;
  }
  return static_cast<Direction>(val);
};

constexpr Direction right_dir(Direction dir) {
  auto val = to_type<Direction>(dir);
  val++;
  if (val > 3) {
    val = 0;
  }
  return static_cast<Direction>(val);
};

struct Point {
public:
  int x;
  int y;

  const bool operator==(const Point &obj) const {
    return obj.x == x && obj.y == y;
  }
  int manhattan_distance(Point p) { return abs(x - p.x) + abs(y - p.y); }
  bool operator<(const Point &rhs) const {
    return (y * 100000 + x) < (rhs.y * 100000 + rhs.x);
  }
};

struct PointCmp {

public:
  constexpr bool operator()(Point a, Point b) const {
    return (a.y * 100000 + a.x) < (b.y * 100000 + b.x);
  }
};

std::ostream &operator<<(std::ostream &out, Point const &point) {
  out << "(" << point.x << "," << point.y << ")";
  return out;
}
namespace std {
template <> struct hash<Point> {

  std::size_t operator()(const Point &p) const {
    std::size_t val{0};
    val = 1000000 * p.y + p.x;
    return val;
  }
};
} // namespace std
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
private:
  int ins_pointer = 0;
  int relative_base = 0;
  std::vector<long long int> outputs;
  std::vector<long long int> program;
  bool return_on_output; // already return if there is an output, not wait for
                         // HALT

  long long int extract_one_rh(Mode mode, long long position) {
    long long int arg1{};
    long long int abs_position{};
    if (mode == Mode::POSITION) {
      abs_position = program[ins_pointer + position];
    } else if (mode == Mode::RELATIVE) {
      abs_position = relative_base + program[ins_pointer + position];
    } else {
      abs_position = ins_pointer + position;
    }
    if (abs_position > program.size()) {
      program.resize(abs_position, 0);
    }
    arg1 = program[abs_position];
    return arg1;
  }

  std::pair<long long int, long long int>
  extract_two_rh(Instruction const &inst) {
    long long int arg1{};
    long long int arg2{};
    arg1 = extract_one_rh(inst.arg1, 1);
    arg2 = extract_one_rh(inst.arg2, 2);
    return std::make_pair(arg1, arg2);
  }

  void assing(const Mode mode, unsigned position, long long int rh) {
    long long int abs_position{};
    if (mode == Mode::POSITION) {
      abs_position = program[ins_pointer + position];

    } else if (mode == Mode::RELATIVE) {
      abs_position = relative_base + program[ins_pointer + position];
    } else {
      abs_position = ins_pointer + position;
    }
    if (abs_position > program.size()) {
      program.resize(abs_position, 0);
    }
    program[abs_position] = rh;
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
    long long int temp = 0;
    long long int arg1, arg2, arg3;
    while (true) {
      Instruction inst = parse_instruction(program[ins_pointer]);
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
};

std::vector<long long int> string2vector(std::stringstream &ss) {
  std::vector<long long int> vec;
  long long int a;
  while (ss >> a) {
    vec.push_back(a);
    if (ss.peek() == ',')
      ss.ignore();
  }
  return vec;
}

void tests() {
  std::vector<long long int> vec;
  std::stringstream ss;
  long long int output;
  bool halted;
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

Point get_next_point(Direction dir, Point &cur_point) {
  switch (dir) {
  case Direction::UP:
    cur_point.y = cur_point.y - 1;
    break;
  case Direction::RIGHT:
    cur_point.x = cur_point.x + 1;
    break;
  case Direction::DOWN:
    cur_point.y = cur_point.y + 1;
    break;
  case Direction::LEFT:
    cur_point.x = cur_point.x - 1;
    break;
  default:
    break;
  }
  return cur_point;
}

std::map<Point, int> run_program(std::vector<long long int> vec,
                                 int first_input) {
  Interpreter interpreter{vec, true};
  std::deque<long long int> inputs{first_input};
  std::vector<int> outputs{};
  bool halted = false;
  int output{};
  std::map<Point, int> point_map{};
  Point cur_point{0, 0};
  Direction dir = Direction::UP;
  while (!halted) {
    std::tie(output, halted) = interpreter.run_programm(inputs);
    outputs.push_back(output);
    if (outputs.size() == 2) {
      Color col = static_cast<Color>(outputs[0]);
      if ((point_map.find(cur_point) != point_map.end()) ||
          (col == Color::WHITE)) {
        point_map[cur_point] = static_cast<int>(col);
      }
      //      std::cout << cur_point << " " << outputs[0] << " " << outputs[1]
      //      << " "
      //                << point_map.size() << "\n";
      if (outputs[1] == 0) {
        dir = left_dir(dir);
      } else if (outputs[1] == 1) {
        dir = right_dir(dir);
      } else {
        std::cerr << "Unkown direction input\n";
      }

      cur_point = get_next_point(dir, cur_point);
      inputs.clear();
      if (point_map.find(cur_point) == point_map.end()) {
        inputs.push_back(static_cast<int>(Color::BLACK));
      } else {
        inputs.push_back(static_cast<int>(point_map[cur_point]));
      }
      outputs.clear();
    }
  }
  return point_map;
}

void render_identifier(std::map<Point, int> point_map) {
  auto top_left = point_map.begin()->first;
  auto buttom_right = point_map.rbegin()->first;
  for (auto y = top_left.y; y <= buttom_right.y; ++y) {
    for (auto x = top_left.x - 1; x <= buttom_right.x; ++x) {
      Point cur_point{x, y};
      if (point_map.find(cur_point) != point_map.end() &&
          point_map[cur_point] == static_cast<int>(Color::WHITE)) {
        std::cout << "█";
      } else {
        std::cout << " ";
      }
    }
    std::cout << "\n";
  }
}

int main() {
  tests();
  std::ifstream infile("./input/input_11.txt");
  std::vector<long long int> vec;
  std::stringstream ss;
  ss << infile.rdbuf();
  vec = string2vector(ss);
  vec.resize(vec.size() * 10, 0);
  auto point_map = run_program(vec, 0);
  std::cout << "Part 1: " << point_map.size() << "\n";
  point_map = run_program(vec, 1);
  std::cout << "Part 2: " << point_map.size() << "\n";
  render_identifier(point_map);
}
