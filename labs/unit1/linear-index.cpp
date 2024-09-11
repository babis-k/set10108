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
std::vector<std::vector<uint32_t>> test_array_vecvec_allocate(int width, int height)
{
    // Declare
    std::vector<std::vector<uint32_t>> array2d_slow;

    // Allocate
    array2d_slow.resize(height);
    for (int y = 0; y < height; ++y)
        array2d_slow[y].resize(width);

    return array2d_slow;
}

void test_array_vecvec_access(int width, int height, std::vector<std::vector<uint32_t>>& array2d_vecvec)
{
    // Fill with data
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            array2d_vecvec[y][x] = make_rgb(x % 256, y % 256, 100);
}

void test_array_vecvec_access_inv(int width, int height, std::vector<std::vector<uint32_t>>& array2d_vecvec)
{
    // Fill with data
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
            array2d_vecvec[y][x] = make_rgb(x % 256, y % 256, 100);
}

// A 2D array using a contiguous chunk of memory, and a linear index to address it
std::vector<uint32_t> test_array_vec1d_allocate(int width, int height)
{
    // Declare the array as a SINGLE vector
    std::vector<uint32_t> array2d;

    // Allocate it to contain width * height elements
    array2d.resize(width * height);

    return array2d;
}

void test_array_vec1d_access(int width, int height, std::vector<uint32_t>& array2d)
{
    // Fill with data, in a ROW MAJOR way
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            int linearIndex = x + y * width;
            array2d[linearIndex] = make_rgb(x % 256, y % 256, 100);
        }
}

void test_array_vec1d_access_inv(int width, int height, std::vector<uint32_t>& array2d)
{
    // Fill with data, in a COLUMN MAJOR way
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
        {
            int linearIndex = x + y * width;
            array2d[linearIndex] = make_rgb(x % 256, y % 256, 100);
        }
}

// Helper stopwatch class
struct stopwatch_t
{
    std::chrono::high_resolution_clock::time_point startTime;

    void start()
    {
        startTime = high_resolution_clock::now();
    }

    void stop_and_report(const char * prefix)
    {
        auto stopTime = high_resolution_clock::now();
        auto total = stopTime - startTime;
        auto totalMs = std::chrono::duration_cast<std::chrono::milliseconds>(total).count();
        cout << prefix << ": " << totalMs << "ms"<<std::endl;
    }
};


int main(int argc, char** argv)
{
    const int numTests = 10;
    const int width = 3840;
    const int height = 2160;

    stopwatch_t stopwatch;
    std::vector<std::vector<uint32_t>> array2d_vecvec;
    std::vector<uint32_t> array2d;

    stopwatch.start();
    for (int i = 0; i < numTests; ++i)
        array2d_vecvec = test_array_vecvec_allocate(width, height);
    stopwatch.stop_and_report("VecVec/Allocate ");

    stopwatch.start();
    for (int i = 0; i < numTests; ++i)
        array2d = test_array_vec1d_allocate(width, height);
    stopwatch.stop_and_report("Vec1d /Allocate ");

    stopwatch.start();
    for (int i = 0; i < numTests; ++i)
        test_array_vecvec_access(width, height, array2d_vecvec);
    stopwatch.stop_and_report("VecVec/Access   ");

    stopwatch.start();
    for (int i = 0; i < numTests; ++i)
        test_array_vec1d_access(width, height, array2d);
    stopwatch.stop_and_report("Vec1d /Access   ");

    stopwatch.start();
    for (int i = 0; i < numTests; ++i)
        test_array_vecvec_access_inv(width, height, array2d_vecvec);
    stopwatch.stop_and_report("VecVec/AccessInv");

    stopwatch.start();
    for (int i = 0; i < numTests; ++i)
        test_array_vec1d_access_inv(width, height, array2d);
    stopwatch.stop_and_report("Vec1d /AccessInv");

    // write the image
    stbi_write_png("test_image.png", width, height, 4, array2d.data(), width * 4);

    return 0;
}