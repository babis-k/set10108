#include <thread>
#include <array>
#include <mutex>
#include <chrono>

using namespace std;
using namespace std::chrono;

// A given class for a pile of clothes. Just use the API!
struct pile_t
{
    // return if we managed to pop a load from the pile (we can pop only if the number is greater than 0)
    bool Pop()
    {
        std::lock_guard<std::mutex> guard(mutex);
        auto ok = num > 0;
        if (num > 0)
            --num;
        return ok;
    }

    // increment the size of the pile
    void Increment()
    {
        std::lock_guard<std::mutex> guard(mutex);
        ++num;
    }

    // get the size of the pile
    int Num() {
        std::lock_guard<std::mutex> guard(mutex);
        return num;
    }

    // set the starting size of the pile
    void Initialize(int startingNumberOfLoads) {
        std::lock_guard<std::mutex> guard(mutex);
        num = startingNumberOfLoads;
    }

private:
    std::mutex mutex;
    int num = 0;
};

// durations for each work (in msec)
constexpr size_t WASH_DUR = 8;
constexpr size_t DRY_DUR = 4;
constexpr size_t IRON_DUR = 2;

// the number of loads
constexpr int MAX_LOADS = 10000;

// our piles
pile_t to_wash_num;
pile_t to_dry_num;
pile_t to_iron_num;
pile_t done_num;

// helper to initialize all piles
void initialize_piles()
{
    // Initially, we have N things to wash, and nothing is done
    to_wash_num.Initialize(MAX_LOADS);
    to_dry_num.Initialize(0);
    to_iron_num.Initialize(0);
    done_num.Initialize(0);
}

/*
*   Task 1: Create the threads & code the logic for the do_work function:
        Create and run the threads for each of wash/dry/iron (1 thread each)
        make sure the threads will be running while there's still work to do (what condition? How frequently to check the condition is up to you)
        do_work function: while we still have potential work to do, Pop a load from the pile, work on it (sleep for the work duration) and add it to the output pile

    Task 2: Measure performance for 1000 loads

    Task 3: Investigate best thread distribution. Have N threads for washing, M threads for drying, P threads for ironing. Which numbers get you the best performance?
        a brute force approach: instead of putting your code in the main function, put it in another function, which you can call with parameters
*   
*/

void do_work(pile_t& pile_in, pile_t& pile_out, int work_duration, const char * name)
{
    // Even if the input pile is empty and the output pile is empty, we might still have work to do if everything is not done
    // We're certain that nothing more is comin, if the "done" pile is full
    while (done_num.Num() != MAX_LOADS)
    {
        // keep trying to pop while the input pile has things to work on
        while (pile_in.Pop())
        {
            // do some work
            this_thread::sleep_for(std::chrono::milliseconds(work_duration));
            // ... and add the item (sort of, by incrementing) to the output pile
            pile_out.Increment();
        }
        //this_thread::sleep_for(std::chrono::milliseconds(1)); // is this better if we sleep for a tiny bit?
    }
}

// our 3 functions
void wash()
{
    do_work(to_wash_num, to_dry_num, WASH_DUR,"wash");
}

void dry()
{
    do_work(to_dry_num, to_iron_num, DRY_DUR,"dry");
}

void iron()
{
    do_work(to_iron_num, done_num, IRON_DUR,"iron");
}

int main(int argc, char **argv)
{
    initialize_piles();

    // create the threads, ensure the program runs until we wash/dry/iron everything, and exit gracefully
    array< thread, 3> threads = { thread(wash),
        thread(dry),
        thread(iron) };

    while (done_num.Num() != MAX_LOADS)
    {
        printf("Checking work status: %d unwashed, %d washed, %d dried, %d done!\n", to_wash_num.Num(), to_dry_num.Num(), to_iron_num.Num(), done_num.Num());
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (auto& t : threads)
        if (t.joinable())
            t.join();

    return 0;
}