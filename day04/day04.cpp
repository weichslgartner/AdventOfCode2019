#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <unordered_set>
#include <iterator>
#include <math.h>

constexpr int pw_len = 6;

constexpr int pow10(int n)
{
    int pow10[] = {
        1, 10, 100, 1000, 10000,
        100000, 1000000, 10000000, 100000000, 1000000000};
    if (n <10)
        return pow10[n];
    else
        return pow(10,n);
}

bool is_valid_password(std::array<int, pw_len> pw, bool part2)
{
    bool same_digit = false;
    bool ascending = true;
    for (int i = 0; i < pw_len - 1; i++)
    {
        bool  temp_same_digit = (pw[i] == pw[i + 1]);
        if (temp_same_digit && part2)
        {
            bool larger_group = false;
            if (i == 0)
            {
                larger_group = (pw[i] == pw[i + 2]);
            }
            else if (i == (pw_len - 2))
            {
                larger_group = (pw[i] == pw[i - 1]);
            }
            else
            {
                larger_group = (pw[i] == pw[i - 1]) || (pw[i] == pw[i + 2]);
            }

            if (larger_group)
            {
                temp_same_digit = false;
            }
        }
        same_digit = same_digit || temp_same_digit;
        ascending = ascending && (pw[i] <= pw[i + 1]);
    }
    return same_digit && ascending;
}

std::array<int, pw_len> int_2_array(int value)
{
    std::array<int, pw_len> pw;
    for (int i = pw_len - 1; i >= 0; --i)
    {
        int temp = value / pow10(i);
        value = value - temp * pow10(i);
        pw[pw_len - 1 - i] = temp;
    }

    return pw;
}

void tests()
{
    assert(is_valid_password(int_2_array(123444), true) == false);
    assert(is_valid_password(int_2_array(111111), false) == true);
    assert(is_valid_password(int_2_array(223450), false) == false);
    assert(is_valid_password(int_2_array(123789), false) == false);
    assert(is_valid_password(int_2_array(111111), true) == false);
    assert(is_valid_password(int_2_array(112233), true) == true);
    assert(is_valid_password(int_2_array(222667), true) == true);
}

int main()
{
    tests();
    std::ifstream infile("../input_04.txt");
    
    std::stringstream ss;
    ss << infile.rdbuf();
    int a;
    std::vector<int> ranges;
    while (ss >> a){
        ranges.push_back(a);
        if (ss.peek() == '-')
            ss.ignore();
    }
    assert(ranges.size()==2);
    int valid_cnt_pt1 = 0;
    int valid_cnt_pt2 = 0;
    for (int i = ranges[0]; i <= ranges[1]; ++i)
    {
        auto value = int_2_array(i);

        if (is_valid_password(value, false))
        {
            valid_cnt_pt1++;
            if (is_valid_password(value, true))
            {
                valid_cnt_pt2++;
            }
        }
    }
    std::cout << "part1: " << valid_cnt_pt1 << "\n";
    std::cout << "part1: " << valid_cnt_pt2 << "\n";
    return 0;
}