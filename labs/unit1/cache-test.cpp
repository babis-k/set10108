#include <vector>
#include <cstdint>
#include <chrono>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace std;
using namespace std::chrono;

// Create an RGB "point". RGB values in [0,255]. Has 3 components, but uses memory for 4, because processing of 32-bit integers is more efficient than processing 24 bits at a time
uint32_t make_rgb(uint32_t r, uint32_t g, uint32_t b)
{
    return r | (g << 8) | (b << 16) | (255 << 24);
}

// Intuitive approach for making 2D arrays
void test_array_slow(int width, int height)
{
    // Declare the array as a SINGLE vector
    std::vector<uint32_t> array2d;

    // Allocate it to contain width * height elements
    array2d.resize(width * height);

    // Fill with data, in a ROW MAJOR way
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            int linearIndex = x + y * width;
            array2d[linearIndex] = make_rgb(x % 256, y % 256, 100);
        }


    // If we provide a filename, save the results to an image
    if (filename != nullptr)
        stbi_write_bmp(filename, width, height, 4, array2d.data());
}

// A 2D array using a contiguous chunk of memory, and a linear index to address it
void test_array_fast(int width, int height, const char* filename = nullptr)
{
    // Declare the array as a SINGLE vector
    std::vector<uint32_t> array2d;

    // Allocate it to contain width * height elements
    array2d.resize(width * height);

    // Fill with data, in a ROW MAJOR way
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            int linearIndex = x + y * width;
            array2d[linearIndex] = make_rgb(x % 256, y % 256, 100);
        }


    // If we provide a filename, save the results to an image
    if (filename != nullptr)
        stbi_write_bmp(filename, width, height, 4, array2d.data());
}


int main(int argc, char** argv)
{
    const int width = 1920;
    const int height = 1080;

    auto start = system_clock::now();
    for (int i = 0; i < 100; ++i)
        test_array_slow(width, height);
    auto end = system_clock::now();
    auto elapsed = (end - start).count();
    std::cout << "Slow: " << elapsed << "\n";

    start = system_clock::now();
    for (int i = 0; i < 100; ++i)
        test_array_fast(width, height);
    end = system_clock::now();
    elapsed = (end - start).count();
    std::cout << "Fast: " << elapsed << "\n";

    test_array_fast(width, height, "test_image.bmp");

    return 0;
}