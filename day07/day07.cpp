#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <deque>
#include <tuple>

template <typename T>
constexpr auto to_type(T e)
{
	return static_cast<typename std::underlying_type<T>::type>(e);
}

enum class Opcode
{
	ADD = 1,
	MULT = 2,
	INPUT = 3,
	OUTPUT = 4,
	JIT = 5,
	JIF = 6,
	LT = 7,
	EQ = 8,
	HALT = 99
};

enum class Mode
{
	POSITION = 0,
	IMMEDIATE = 1
};

struct Instruction
{
	Opcode opcode;
	Mode arg1;
	Mode arg2;
	Mode arg3;

public:
	Instruction(Opcode op, Mode a1 = Mode::POSITION, Mode a2 = Mode::POSITION,
				Mode a3 = Mode::POSITION)
	{
		opcode = op;
		arg1 = a1;
		arg2 = a2;
		arg3 = a3;
	}
};

Instruction parse_instruction(int inst)
{
	std::string str_inst = std::to_string(inst);
	str_inst = std::string(5 - str_inst.length(), '0') + str_inst;
	auto op = static_cast<Opcode>(std::stoi(str_inst.substr(3, 2)));
	auto a1 = static_cast<Mode>(std::stoi(str_inst.substr(2, 1)));
	auto a2 = static_cast<Mode>(std::stoi(str_inst.substr(1, 1)));
	auto a3 = static_cast<Mode>(std::stoi(str_inst.substr(0, 1)));
	return Instruction(op, a1, a2, a3);
}

template <typename T>
void print_vector(std::vector<T> vec)
{
	for (auto v : vec)
	{
		std::cout << v << " ";
	}
	std::cout << "\n";
}

class Interpreter
{
private:
	int ins_pointer = 0;
	std::vector<int> outputs;
	std::vector<int> program;
	bool return_on_output; // already return if there is an output, not wait for HALT

	int extract_one_rh(Mode mode, int position)
	{

		int arg1{};

		if (mode == Mode::POSITION)
		{
			arg1 = program[program[ins_pointer + position]];
		}
		else
		{
			arg1 = program[ins_pointer + position];
		}
		return arg1;
	}

	std::pair<int, int> extract_two_rh(Instruction const &inst)
	{

		int arg1{};
		int arg2{};
		arg1 = extract_one_rh(inst.arg1, 1);
		arg2 = extract_one_rh(inst.arg2, 2);
		return std::make_pair(arg1, arg2);
	}

	void assing(const Mode mode, unsigned position, int rh)
	{
		if (mode == Mode::POSITION)
		{
			program[program[ins_pointer + position]] = rh;
		}
		else
		{
			program[ins_pointer + position] = rh;
		}
	}

public:
	Interpreter(std::vector<int> vec, bool rop = true)
	{
		program = vec;
		return_on_output = rop;
	}
	// returns output and if program is halted
	std::pair<int, bool> run_programm(std::deque<int> &inputs)
	{
		//int ins_pointer = 0;
		int temp = 0;
		int arg1, arg2, arg3;
		//std::vector<int> outputs;
		//ins_pointer = 0;
		while (true)
		{
			Instruction inst = parse_instruction(program[ins_pointer]);
			switch (inst.opcode)
			{
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
				if (arg1 != 0)
				{
					ins_pointer = arg2;
				}
				else
				{
					ins_pointer += 3;
				}
				break;
			case Opcode::JIF:
				std::tie(arg1, arg2) = extract_two_rh(inst);
				if (arg1 == 0)
				{
					ins_pointer = arg2;
				}
				else
				{
					ins_pointer += 3;
				}
				break;
			case Opcode::LT:
				std::tie(arg1, arg2) = extract_two_rh(inst);
				if (arg1 < arg2)
				{
					assing(inst.arg3, 3, 1);
				}
				else
				{
					assing(inst.arg3, 3, 0);
				}
				ins_pointer += 4;
				break;
			case Opcode::EQ:
				std::tie(arg1, arg2) = extract_two_rh(inst);
				if (arg1 == arg2)
				{
					assing(inst.arg3, 3, 1);
				}
				else
				{
					assing(inst.arg3, 3, 0);
				}
				ins_pointer += 4;
				break;
			case (Opcode::HALT):
				//print_vector(program);
				//assert(outputs.size() == 1);
				return std::make_pair(outputs[outputs.size() - 1], true);
				//return std::pair(0, true);
			default:
				std::cout << "Unkown Opcode " << program[ins_pointer] << "\n";
				break;
			}
		}
		// never reached
		return std::make_pair(-1, false);
	}
};

std::vector<int> string2vector(std::stringstream &ss)
{
	std::vector<int> vec;
	int a;
	while (ss >> a)
	{
		vec.push_back(a);
		if (ss.peek() == ',')
			ss.ignore();
	}
	return vec;
}

void tests()
{
}

int eval_amp(std::vector<int> vec, int i, int j, int k, int l, int m,
			 bool part2)
{
	int result_e = 0;
	bool halted_e = false;
	Interpreter inter_a{vec};
	Interpreter inter_b{vec};
	Interpreter inter_c{vec};
	Interpreter inter_d{vec};
	Interpreter inter_e{vec};
	std::deque<int> input_a;
	std::deque<int> input_b;
	std::deque<int> input_c;
	std::deque<int> input_d;
	std::deque<int> input_e;
	input_a.push_back(i);
	input_b.push_back(j);
	input_c.push_back(k);
	input_d.push_back(l);
	input_e.push_back(m);

	do
	{
		input_a.push_back(result_e);
		auto [result_a, halted_a] = inter_a.run_programm(input_a);
		input_b.push_back(result_a);
		auto [result_b, halted_b] = inter_b.run_programm(input_b);
		input_c.push_back(result_b);
		auto [result_c, halted_c] = inter_c.run_programm(input_c);
		input_d.push_back(result_c);
		auto [result_d, halted_d] = inter_d.run_programm(input_d);
		input_e.push_back(result_d);
		std::tie(result_e, halted_e) = inter_e.run_programm(input_e);
	} while (part2 && halted_e == false);
	return result_e;
}

int find_highest_output(std::vector<int> vec, std::pair<int, int> range,
						bool part1)
{
	int max_result = 0;
	for (int i = range.first; i <= range.second; ++i)
	{

		for (int j = range.first; j <= range.second; ++j)
		{
			if (j == i)
				continue;

			for (int k = range.first; k <= range.second; ++k)
			{
				if (k == i || k == j)
					continue;

				for (int l = range.first; l <= range.second; ++l)
				{
					if (l == i || l == k || l == j)
						continue;

					for (int m = range.first; m <= range.second; ++m)
					{
						if (m == l || m == i || m == k || m == j)
							continue;
						int temp_result = 0;
						if (part1)
						{
							temp_result = eval_amp(vec, i, j, k, l, m, false);
						}
						else
						{
							temp_result = eval_amp(vec, i, j, k, l, m, true);
						}
						if (temp_result > max_result)
						{
							max_result = temp_result;
						}
					}
				}
			}
		}
	}

	//std::cout << max_result;
	return max_result;
}

int main()
{
	tests();
	std::ifstream infile("../input_07.txt");
	std::vector<int> vec;
	std::stringstream ss;
	ss << infile.rdbuf();
	vec = string2vector(ss);
	int highest = find_highest_output(vec, std::make_pair(0, 4), true);
	std::cout << "Part 1: " << highest << "\n";
	highest = find_highest_output(vec, std::make_pair(5, 9), false);
	std::cout << "Part 2: " << highest << "\n";
	return 0;
}
