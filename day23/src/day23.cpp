#include <assert.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <deque>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <iterator>
#include <array>

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

	explicit Instruction(Opcode op, Mode a1 = Mode::POSITION, Mode a2 = Mode::POSITION, Mode a3 = Mode::POSITION) {
		opcode = op;
		arg1 = a1;
		arg2 = a2;
		arg3 = a3;
	}
};

Instruction parse_instruction(int inst) {
	auto str_inst { std::to_string(inst) };
	str_inst = std::string(5 - str_inst.length(), '0') + str_inst;
	auto op = static_cast<Opcode>(std::stoi(str_inst.substr(3, 2)));
	auto a1 = static_cast<Mode>(std::stoi(str_inst.substr(2, 1)));
	auto a2 = static_cast<Mode>(std::stoi(str_inst.substr(1, 1)));
	auto a3 = static_cast<Mode>(std::stoi(str_inst.substr(0, 1)));
	return Instruction(op, a1, a2, a3);
}

template<typename T> void print_vector(std::vector<T> vec) {
	fmt::print("{}\n", fmt::join(vec, " "));
}

class Interpreter {
	int ins_pointer = 0;
	long long int relative_base = 0;
	std::vector<long long int> outputs { };
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
			fmt::print("{} ", arg1);
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
		if (VERBOSE) {
			fmt::print("abs pos {} ", abs_position);
		}
	}

public:
	Interpreter() :
			return_on_output { true } {

	}
	explicit Interpreter(std::vector<long long int> vec, bool rop = true) :
			program { std::move(vec) }, return_on_output { rop } {

	}
	// returns output and if program is halted
	std::pair<long long int, bool> run_programm(std::deque<long long int> &inputs) {
		//fmt::print("ins {}\n",ins_pointer);
		long long int temp { 0 };
		long long int arg1 { }, arg2 { }, arg3 { };
		while (true) {
			if (ins_pointer < 0) {
				fmt::print("ALARM\n");
				return std::make_pair(arg1, false);
			}

			Instruction inst = parse_instruction(program[ins_pointer]);
			if (VERBOSE) {
				fmt::print("\n{} {} {} ", to_type(inst.opcode), ins_pointer, relative_base);
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
				//assert(inputs.size() > 0);
				if (inputs.empty())
					return std::make_pair(-1, false);
				temp = inputs.front();
				assing(inst.arg1, 1, temp);
				inputs.pop_front();
				ins_pointer += 2;
				break;
			case Opcode::OUTPUT:
				arg1 = extract_one_rh(inst.arg1, 1);
				outputs.push_back(arg1);
				ins_pointer += 2;
				if (return_on_output){
					return std::make_pair(arg1, false);
				}
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
				fmt::print(stderr, "Unkown Opcode {}\n", program[ins_pointer]);
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
		if (abs_position < 0) {
			fmt::print(stderr, "ALARM pos{}\n", abs_position);

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

struct computer {
	Interpreter interpreter;
	std::deque<long long int> inputs;
	long long output;
	std::deque<long long int> outputs;
	bool halted;
};


std::pair<long long, long long> do_networking(std::vector<long long int>  &vec){
	constexpr auto n_computers{50};
	constexpr auto nat_adress{255};
	std::array<computer, n_computers> computers { };
	for (auto i { 0 }; i < n_computers; i++) {
		computers[i] = { Interpreter { vec }, std::deque<long long int> { i }, -1, std::deque<long long int>{ }, false };
	}
	auto nat_x{0LL};
	auto nat_y{0LL};
	auto part1{0LL};
	auto part2{0LL};
	int last_send=-1;
	while (true) {
		bool idle{true};
		for (auto i { 0 }; i < n_computers; i++) {
			computers[i].output = 0;
			while (computers[i].output != -1) {
				if (computers[i].inputs.empty()) {
					computers[i].inputs.push_back(-1);
				}else{
					idle=false;
				}
				std::tie(computers[i].output, computers[i].halted) = computers[i].interpreter.run_programm(computers[i].inputs);
				if(computers[i].output != -1)
					computers[i].outputs.push_back(computers[i].output);
			}
			while (computers[i].outputs.size() > 0) {
				idle=false;
				auto address = computers[i].outputs.front();
				computers[i].outputs.pop_front();
				auto X = computers[i].outputs.front();
				computers[i].outputs.pop_front();
				auto Y = computers[i].outputs.front();
				computers[i].outputs.pop_front();
				if (address < n_computers) {
					computers[address].inputs.push_back(X);
					computers[address].inputs.push_back(Y);
				} else if (address == nat_adress){
					if(part1==0LL){
						part1 = Y;
					}
					nat_x = X;
					nat_y = Y;

				}
				last_send = i;
			}

		}//end for
		if(idle and nat_x!= 0 and nat_y != 0){
			computers[0].inputs.push_back(nat_x);
			computers[0].inputs.push_back(nat_y);
			if(last_send==nat_adress){
				part2 = nat_y;
				break;
			}
			last_send = nat_adress;
		}
	}
	return std::make_pair(part1,part2);
}

int main() {
	constexpr auto filename { "build/input/input_23.txt" };
	std::ifstream infile(filename);
	if (not infile.is_open()) {
		fmt::print(stderr, "can't open file {}", filename);
	}
	std::stringstream ss { };
	ss << infile.rdbuf();
	auto vec = string2vector(ss);
	auto [part1,part2] = do_networking(vec);
	fmt::print("Part1: {}\n",part1);
	fmt::print("Part2: {}\n",part2);
	return EXIT_SUCCESS;
}

