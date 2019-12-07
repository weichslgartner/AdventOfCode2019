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
    HALT = 99
};

template <typename T>
void print_vector(std::vector<T> vec)
{
    for (auto v : vec)
    {
        std::cout << v << " ";
    }
    std::cout << "\n";
}

int run_programm(std::vector<int> program)
{
    int ins_pointer = 0;
    int temp = 0;
    while (true)
    {

        switch (program[ins_pointer])
        {
        case to_type(Opcode::ADD):
            temp = program[program[ins_pointer + 1]] + program[program[ins_pointer + 2]];
            program[program[ins_pointer + 3]] = temp;
            break;
        case to_type(Opcode::MULT):
            temp = program[program[ins_pointer + 1]] * program[program[ins_pointer + 2]];
            program[program[ins_pointer + 3]] = temp;
            break;
        case to_type(Opcode::HALT):
            //print_vector(program);
            return program[0];

        default:
            std::cout << "Unkown Opcode " << program[ins_pointer] << "\n";
            break;
        }
        ins_pointer += 4;
    }
    // never reached
    return -1;
}

void set_noun_verb(std::vector<int> &vec, int noun, int verb)
{
    assert(vec.size() > 2);
    vec[1] = noun;
    vec[2] = verb;
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
    std::stringstream ss{"1,0,0,0,99"};
    std::vector<int> vec = string2vector(ss);
    assert(run_programm(vec) == 2);
    std::stringstream ss1{"1,1,1,4,99,5,6,0,99"};
    std::vector<int> vec1 = string2vector(ss1);
    assert(run_programm(vec1) == 30);
}

int main()
{
    tests();
    std::ifstream infile("../input_02.txt");
    std::vector<int> vec, temp_vec;
    std::stringstream ss;
    ss << infile.rdbuf();
    vec = string2vector(ss);
    temp_vec = vec;
    // part 1
    set_noun_verb(temp_vec, 12, 2);
    int result = run_programm(temp_vec);
    std::cout << "result (part 1): " << result << "\n";
    // part 2
    for (int noun = 0; noun < 100; ++noun)
    {
        for (int verb = 0; verb < 100; ++verb)
        {
            temp_vec = vec;
            set_noun_verb(temp_vec, noun, verb);
            result = run_programm(temp_vec);
            if (result == 19690720)
            {
                std::cout << "pair (part 2): " << noun * 100 + verb << "\n";
                return 0;
            }
        }
    }
    return 0;
}