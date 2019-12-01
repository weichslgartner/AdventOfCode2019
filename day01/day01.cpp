#include <iostream>
#include <fstream>

constexpr int calc_fuel(const int input)
{
    int result = input / 3;
    result -= 2;
    if (result < 0)
        result = 0;
    return result;
}

constexpr int calc_fuel_b(const int input)
{
    int result = 0;
    int tmp = input;
    while ((tmp = calc_fuel(tmp)) > 0)
    {
        result += tmp;
    }
    return result;
}

void tests()
{
    static_assert(calc_fuel(12) == 2);
    static_assert(calc_fuel(14) == 2);
    static_assert(calc_fuel(1969) == 654);
    static_assert(calc_fuel(100756) == 33583);
    static_assert(calc_fuel_b(1969) == 966);
    static_assert(calc_fuel_b(12) == 2);
    static_assert(calc_fuel_b(100756) == 50346);
}

int main()
{
    tests();
    std::ifstream infile("../input_01.txt");

    int fuel_a = 0;
    int fuel_b = 0;
    int a;
    while (infile >> a)
    {
        fuel_a += calc_fuel(a);
        fuel_b += calc_fuel_b(a);
    }
    std::cout << "Solution a: " << fuel_a << "\n";
    std::cout << "Solution b: " << fuel_b << "\n";
}