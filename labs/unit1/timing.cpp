#include <chrono>
#include <iostream>
#include <fstream>

using namespace std;
using namespace std::chrono;

void do_work (int& n)
{
    // Do some spinning - no actual processing but will make the CPU work, as long as it's not obvious that we're ignoring the result of n
    n = 0;
    for (int i = 0; i < 1000000; ++i)
        ++n;
}

int main(int argc, char **argv)
{
    // Create a new file
    ofstream data("data.csv", ofstream::out);
    uint64_t total_ns = 0;
    // We’re going to gather 100 readings , so create a thread and join it 100 times
    int n=0;
    for (int i = 0; i < 100; ++i)
    {
        // Get start time
        auto start = system_clock::now();
        do_work(n);
        // Get end time
        auto end = system_clock::now();
        // Calculate the duration
        auto total = end - start;
        // Write to file
        total_ns += total.count();
        data << total.count() << endl;
    }
    // 100 iterations complete.
    data.close();
    cout << "Mean: " << total_ns/100 << endl;
    return 0;
}