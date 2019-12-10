#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <array>

const std::string input_filename = "../input_08.txt";

class Image
{
private:
    int _width;
    int _height;
    std::vector<int> _vec;

public:
    Image(std::vector<int> vec, const int width, const int height)
    {
        _width = width;
        _height = height;
        _vec = vec;
    }

    int find_fewest_digits(int digit)
    {
        assert(digit < 10);
        auto size = _width * _height;
        int cnt = 0;
        int result = 0;
        std::array<int, 10> cnt_array = {};
        int min_digit = INT32_MAX;
        for (auto element : _vec)
        {
            assert(element < 10);
            cnt_array[element]++;
            cnt++;
            if (cnt == size)
            {
                if (cnt_array[digit] < min_digit)
                {
                    min_digit = cnt_array[digit];
                    result = cnt_array[1] * cnt_array[2];
                }
                cnt_array.fill(0);
                cnt = 0;
            }
        }
        return result;
    }

    void render_image()
    {
        std::vector<int> vis_img(_width * _height, -1);
        int cnt = 0;
        for (auto element : _vec)
        {
            if (element != 2 && vis_img[cnt] == -1)
            {
                vis_img[cnt] = element;
            }

            cnt++;
            if (cnt == _width * _height)
            {
                cnt = 0;
            }
        }
        cnt = 0;
        for (auto pixel : vis_img)
        {
            switch (pixel)
            {
            case 1:
                std::cout << "█";
                break;
            case 0:
                std::cout << " ";
                break;
            default:
                break;
            }

            cnt++;
            if (cnt == _width)
            {
                std::cout << "\n";
                cnt = 0;
            }
        }
    }
};

std::vector<int> string2vector(std::stringstream &ss)
{
    std::vector<int> vec;
    char a;
    while (ss >> a)
    {
        vec.push_back(a - '0');
    }
    return vec;
}
void tests()
{
    std::ifstream infile(input_filename);
    std::vector<int> vec;
    std::stringstream ss;
    ss << infile.rdbuf();
    vec = string2vector(ss);
    auto image = Image(vec, 25, 6);
    auto min_digets = image.find_fewest_digits(0);
    assert(min_digets == 1560);
}

int main()
{
    tests();
    std::ifstream infile(input_filename);
    std::vector<int> vec;
    std::stringstream ss;
    ss << infile.rdbuf();
    vec = string2vector(ss);
    auto image = Image(vec, 25, 6);
    auto min_digets = image.find_fewest_digits(0);
    std::cout << "Part 1:" << min_digets << "\n";
    image.render_image();
    return 0;
}