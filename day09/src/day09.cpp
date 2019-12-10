#include <assert.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

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
    // int ins_pointer = 0;
    long long int temp = 0;
    long long int arg1, arg2, arg3;
    // std::vector<int> outputs;
    // ins_pointer = 0;
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

int main() {
  tests();
  std::ifstream infile("./input/input_09.txt");
  std::vector<long long int> vec;
  std::stringstream ss;
  ss << infile.rdbuf();
  vec = string2vector(ss);
  vec.resize(vec.size() * 10, 0);
  Interpreter interpreter{vec, true};
  std::deque<long long int> inputs{1};
  bool halted = false;
  long long int output{};
  std::tie(output, halted) = interpreter.run_programm(inputs);
  std::cout << "Part 1: " << output << "\n";
  inputs.clear();
  inputs.push_back(2);
  interpreter = Interpreter{vec, true};
  std::tie(output, halted) = interpreter.run_programm(inputs);
  std::cout << "Part 2: " << output << "\n";
  return 0;
}
