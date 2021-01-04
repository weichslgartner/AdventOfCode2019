#include <assert.h>
#include <fmt/core.h>
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
#include <utility>
#include <vector>

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

namespace std {
template<>
struct hash<Point> {
	std::size_t operator()(const Point &p) const {

		return std::hash<int> { }(p.x) ^ std::hash<int> { }(p.y);
	}
};
}
constexpr auto VERBOSE { false };

template<typename T> constexpr auto to_type(T e) {
	return static_cast<typename std::underlying_type<T>::type>(e);
}

enum class Opcode {
	ADD = 1, MULT = 2, INPUT = 3, OUTPUT = 4, JIT = 5, JIF = 6, LT = 7, EQ = 8, SET_RB = 9, HALT = 99
};

enum class Mode {
	POSITION = 0, IMMEDIATE = 1, RELATIVE = 2
};

struct Instruction {
	Opcode opcode;
	Mode arg1;
	Mode arg2;
	Mode arg3;

public:
	Instruction(Opcode op, Mode a1 = Mode::POSITION, Mode a2 = Mode::POSITION, Mode a3 = Mode::POSITION) {
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

template<typename T> void print_vector(std::vector<T> vec) {
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
	long long int extract_one_rh(Mode const mode, long long position) {
		long long int abs_position { };
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

	std::pair<long long int, long long int> extract_two_rh(Instruction const &inst) {
		auto arg1 = extract_one_rh(inst.arg1, 1);
		auto arg2 = extract_one_rh(inst.arg2, 2);
		return std::make_pair(arg1, arg2);
	}

	void assing(const Mode mode, unsigned position, long long int rh) {
		long long int abs_position { };
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
	Interpreter() {
		return_on_output = true;
	}
	Interpreter(std::vector<long long int> vec, bool rop = true) : program{std::move(vec)}, return_on_output{rop} {

	}
	// returns output and if program is halted
	std::pair<long long int, bool> run_programm(std::deque<long long int> &inputs) {
		long long int temp { 0 };
		long long int arg1 { }, arg2 { }, arg3 { };
		while (true) {
			Instruction inst = parse_instruction(program[ins_pointer]);
			if (VERBOSE) {
				std::cout << "\n";
				std::cout << to_type(inst.opcode) << " " << ins_pointer << " " << relative_base << " ";
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
	std::vector<long long int> vec { };
	long long int a { };
	while (ss >> a) {
		vec.push_back(a);
		if (ss.peek() == ',')
			ss.ignore();
	}
	return vec;
}

int query_program(Point const &&p, std::vector<long long int> const &vec) {
	std::deque<long long int> inputs {p.x,p.y };

	Interpreter interpreter { vec, true };
	auto[output, halted] = interpreter.run_programm(inputs);
	return output;
}


int find_square(std::vector<long long int> const &vec, int size =100) {
	auto xmin { 0 };
	for (auto y { 100 }; y < 10000; y++) {
			auto x = xmin;
			while (query_program(Point { x, y }, vec) == 0) {
				xmin = x;
				x++;
			}
			while(query_program(Point { x + size - 1, y }, vec) == 1 and query_program(Point { x, y }, vec)==1){
				if(query_program(Point { x , y+ size - 1 }, vec) == 1){
					return x*10000 + y;
				}
				x++;
			}


	}
	return -1;
}

int main() {
	constexpr auto filename { "build/input/input_19.txt" };
	std::ifstream infile(filename);
	if (not infile.is_open()) {
		std::cerr << "can't open file " << filename;
	}
	std::stringstream ss { };
	ss << infile.rdbuf();
	auto vec = string2vector(ss);
	auto cnt { 0 };
	for (auto y { 0 }; y < 50; y++) {
		for (auto x { 0 }; x < 50; x++) {
			cnt += query_program(Point{x,y},vec);
		}
	}
	fmt::print("Part 1: {}\n", cnt);
	auto part2 = find_square(vec);
	fmt::print("Part 2: {}\n", part2);
	return EXIT_SUCCESS;
}

