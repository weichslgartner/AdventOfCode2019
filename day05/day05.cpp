#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>

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
    Instruction(Opcode op, Mode a1 = Mode::POSITION, Mode a2 = Mode::POSITION, Mode a3 = Mode::POSITION)
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

int run_programm(std::vector<int> program, int input)
{
    int ins_pointer = 0;
    int temp = 0;
    int arg1, arg2, arg3;
    std::vector<int> outputs;
    while (true)
    {
        Instruction inst = parse_instruction(program[ins_pointer]);
        switch (inst.opcode)
        {
        case (Opcode::ADD):
            if (inst.arg1 == Mode::POSITION)
            {
                arg1 = program[program[ins_pointer + 1]];
            }
            else
            {
                arg1 = program[ins_pointer + 1];
            }
            if (inst.arg2 == Mode::POSITION)
            {
                arg2 = program[program[ins_pointer + 2]];
            }
            else
            {
                arg2 = program[ins_pointer + 2];
            }
            temp = arg1 + arg2;
            if (inst.arg3 == Mode::POSITION)
            {
                program[program[ins_pointer + 3]] = temp;
            }
            else
            {
                program[ins_pointer + 3] = temp;
            }
            ins_pointer += 4;
            break;
        case (Opcode::MULT):
            if (inst.arg1 == Mode::POSITION)
            {
                arg1 = program[program[ins_pointer + 1]];
            }
            else
            {
                arg1 = program[ins_pointer + 1];
            }
            if (inst.arg2 == Mode::POSITION)
            {
                arg2 = program[program[ins_pointer + 2]];
            }
            else
            {
                arg2 = program[ins_pointer + 2];
            }
            temp = arg1 * arg2;
            if (inst.arg3 == Mode::POSITION)
            {
                program[program[ins_pointer + 3]] = temp;
            }
            else
            {
                program[ins_pointer + 3] = temp;
            }
            ins_pointer += 4;
            break;
        case Opcode::INPUT:
            if (inst.arg1 == Mode::POSITION)
                program[program[ins_pointer + 1]] = input;
            else
                program[ins_pointer + 1] = input;

            ins_pointer += 2;
            break;
        case Opcode::OUTPUT:
            if (inst.arg1 == Mode::POSITION)
                arg1 = program[program[ins_pointer + 1]];
            else
                arg1 = program[ins_pointer + 1];

            if (arg1 != 0)
                std::cout << arg1 << '\n';
            ins_pointer += 2;
            break;
        case Opcode::JIT:
            if (inst.arg1 == Mode::POSITION)
            {
                arg1 = program[program[ins_pointer + 1]];
            }
            else
            {
                arg1 = program[ins_pointer + 1];
            }
            if (inst.arg2 == Mode::POSITION)
            {
                arg2 = program[program[ins_pointer + 2]];
            }
            else
            {
                arg2 = program[ins_pointer + 2];
            }
            if (arg1 != 0)
            {
                ins_pointer = arg2;
            }else{
                ins_pointer += 3;
            }
            break;
        case Opcode::JIF:
            if (inst.arg1 == Mode::POSITION)
            {
                arg1 = program[program[ins_pointer + 1]];
            }
            else
            {
                arg1 = program[ins_pointer + 1];
            }
            if (inst.arg2 == Mode::POSITION)
            {
                arg2 = program[program[ins_pointer + 2]];
            }
            else
            {
                arg2 = program[ins_pointer + 2];
            }
            if (arg1 == 0)
            {
                ins_pointer = arg2;
            }else{
                ins_pointer += 3;
            }
            break;
        case Opcode::LT:
            if (inst.arg1 == Mode::POSITION)
            {
                arg1 = program[program[ins_pointer + 1]];
            }
            else
            {
                arg1 = program[ins_pointer + 1];
            }
            if (inst.arg2 == Mode::POSITION)
            {
                arg2 = program[program[ins_pointer + 2]];
            }
            else
            {
                arg2 = program[ins_pointer + 2];
            }
            if (arg1 < arg2)
            {
                if (inst.arg3 == Mode::POSITION)
                {
                    program[program[ins_pointer + 3]] = 1;
                }
                else
                {
                    program[ins_pointer + 3] = 1;
                }
            }
            else
            {
                if (inst.arg3 == Mode::POSITION)
                {
                    program[program[ins_pointer + 3]] = 0;
                }
                else
                {
                    program[ins_pointer + 3] = 0;
                }
            }
            ins_pointer +=4;
            break;
        case Opcode::EQ:
            if (inst.arg1 == Mode::POSITION)
            {
                arg1 = program[program[ins_pointer + 1]];
            }
            else
            {
                arg1 = program[ins_pointer + 1];
            }
            if (inst.arg2 == Mode::POSITION)
            {
                arg2 = program[program[ins_pointer + 2]];
            }
            else
            {
                arg2 = program[ins_pointer + 2];
            }
            if (arg1 == arg2)
            {
                if (inst.arg3 == Mode::POSITION)
                {
                    program[program[ins_pointer + 3]] = 1;
                }
                else
                {
                    program[ins_pointer + 3] = 1;
                }
            }
            else
            {
                if (inst.arg3 == Mode::POSITION)
                {
                    program[program[ins_pointer + 3]] = 0;
                }
                else
                {
                    program[ins_pointer + 3] = 0;
                }
            }
            ins_pointer +=4;
            break;
        case (Opcode::HALT):
            //print_vector(program);
            return program[0];

        default:
            std::cout << "Unkown Opcode " << program[ins_pointer] << "\n";
            break;
        }
    }
    // never reached
    return -1;
}

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

int main()
{
    tests();
    std::ifstream infile("../input_05.txt");
    std::vector<int> vec, vec2;
    std::stringstream ss;
    ss << infile.rdbuf();
    vec = string2vector(ss);
    vec2 = vec;
    run_programm(vec,1);
    run_programm(vec2,5);
    return 0;
}