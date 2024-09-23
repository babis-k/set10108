# Unit 1: Basics

The aim of the first lab is to get you started with performance measurements in C++ using CMake to drive our project generation. Performance is of paramount importance, as it's the main reason we are using parallelism in the first place. This lab will cover some basic performance measurement approaches, some common pitfalls when dealing with multi-dimensional arrays, and an introduction to lambda expressions

## Gathering Data

Throughout the module, we will gather data to analyse the performance of our applications.  The key metrics in parallel performance are *speed up* and *efficiency*. Our method will be to output timing data into a comma separated value (.csv) file. CSV files can be loaded into Excel, R or other data analysis tools, and thereby generate a chart. In our first application, we will get the average of 100 iterations of a simple task that will just spin the processor.

### Work Function

Our work function is below:

```cpp
void work()
{
    // Do some spinning - no actual processing but will make the CPU work
    int n = 0;
    for (int i = 0; i < 1000000; ++i)
        ++n;
}
```

The work is to increment a value 1 million times. This won't take much time for the processor.

### Creating a File

To create an output file in C++ we use the following:

```cpp
ofstream data("data.csv", ofstream::out);
```

An output file called `data.csv` will be created which we can write to.  Filestreams are just normal output streams like `cout`.

### Capturing Times

The `system_clock` type allows time capture in C++. Timing requires the *start* and *end* of an experiment from which we calculate the *duration*.  Timing data will be in *nanoseconds* (ns).

```cpp
// Get start time
auto start = system_clock::now();
// ... do some work
// Get end time
auto end = system_clock::now();
// Calculate total time
auto total = end - start;
// Output total time in ms to the file
data << total.count() << endl;
```

### Complete Application

Our complete application is below:

```cpp
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
    // We’re going to gather 100 readings , so do some (meaningless) work 100 times
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
```

### Getting the Data

Once the application is complete you will have a `.csv` file which you should open with Excel. Get the mean of the 100 stored values.  You also need to document the specification of your machine. This is important information. For example, the resulting average from my test is 11883ns and the hardware I ran the experiment on was:

* **CPU** AMD Ryzen 5 1600 @ 3.3GHz
* **Memory** 8GB
* **OS** Windows 10 64bit
* **Compiler** MSVC 16

As we progress through the module, we will find that the other pieces of information about your machine will become useful.

## Linear index, cache and performance

(The accompanying code for this section is in ```linear-index.cpp```)

With GPU programming and when dealing with images, we have to use multi-dimensional arrays. For example, an image is a 2D array of pixel values. How we represent and process such arrays can have a critical effect on performance, and the reason is *cache performance* (cache hits and cache misses). 

Cache memory is small and fast. When we access memory (e.g. a variable, or an element of an array), the CPU checks if the data is in cache, and if it's not, it copies the data **plus neighbouring data** to the cache, so that next time we access that or nearby memory, it will be fetched from the cache. If the data is not in the cache (*cache miss*), there is an associated perfomance penalty to access slower memory and copy the data to the cache. We should always aim to minimise cache misses.

Below, we focus on 2D arrays, but the concepts are applicable in any number of dimensions.

### Creating 2D arrays

When asked to create a 2D array with a dynamic width and height, an intuitive approach might be to create a vector of vectors:

```cpp
std::vector<std::vector<MyType>> array2d_vecvec;
```

Assuming a desired row-major representation, each inner vector will represent a row, and the outer vector is the collection of all rows. This is not a very efficient representation, for two reasons:

* When allocating the array, instead of allocating once, we allocate ```N+1``` times: once for the outer vector-of-vectors, and once for each row. Allocation of memory from the heap is an expensive process, so this could have a performance impact.
* The inner vectors will likely be stored in completely different areas in memory, scattered, which means we can't use the cache effectively

The entire point of specifying the datatype as a vector of vectors for a 2D array, is to be able to access elements as ```array2d_vecvec[i][j]```

### Using a 1D array to represent 2D arrays

In the case of 2D arrays, we know in advance that we have ```width * height``` elements. Therefore, we can represent a 2D array by using a 1D array and converting the row and column indices to a single, flattened index (AKA linear index). Assuming a row-major order, we calculate the linear index as follows: ```linearIndex = row * width + column```. The benefits of this representation are multiple: 

* We only need to allocate heap memory once: ```width * height``` elements.
* The data structure is cache friendly: if we process all elements one-by-one, in a row-major order, then every access will most likely be a cache hit, even if we change to a different row, because all rows are now stored closely together.
* We can use a single type (std::vector) to represent multi-dimensional arrays of any dimension. 

### Traversing arrays in the wrong order

Consider the following code:

```
std::vector<std::vector<SomeType>> array2d_vecvec;
// ... allocate the outer and inner vectors

// Fill with data in a row-major order
for (int y = 0; y < height; ++y)
	for (int x = 0; x < width; ++x)
		array2d_vecvec[y][x] = ...;

// Fill with data in a column-major order
for (int x = 0; x < width; ++x)
	for (int y = 0; y < height; ++y)
		array2d_vecvec[y][x] = ...;
```

What happens in the second example? Let's break it down, in the context of cache

* access x==0, y==0: not in cache, so copy the entire vector (all elements where y==0)
* access x==0, y==1: not in cache, so copy the entire vector. We have all elements where y==0 already in cache, but we jumped an entire row, and the new row's elements are not in cache yet. So, we copy them here.
* access x==0, y==2: as above
* ...
* access x==1, y==0: probably not in cache, as all the previous accesses would have caused all other rows to get copied into the cache, and we might have run out of space, deleting the oldest entries first (where y==0)
* access x==1, y==1: as above
* ....

So this is a worst case scenario. Accessing the multi-dimensional arrays in the wrong order is also very destructive for performance when using the 1D array approach above.
All this is demonstrated in code, in the ```linear-index``` application. Change the dimensions of the array to notice different effects: when the 2D array is small enough to fit in cache (e.g. 1000x1000), there is minimal performance penalty from incorrect traversal order. But, the bigger the array, the bigger the effect.

### Why are we talking about performance in the first lab?

You might be wondering this, so here's a reminder: we use parallelism for improving performance in our application. If you run the test application, and use big enough values for the array size (e.g. 10000 x 10000) you will realize that good versus bad use of the cache can result in the application running 10 times faster or slower! 

## Lambda (λ)-Expressions

Lambda (λ)-expressions are becoming popular in object-oriented languages. λ-expressions come from functional languages, e.g. F#, Haskell, etc.  They allow function objects which we can apply parameters to and get a result.

### What is a λ-Expression

A λ-expression is essentially a function.  However, it has some properties that allow us to manipulate the function. For example, if we define a function as follows:

```
add(x, y) = x + y
```

We can use this function object to create a new function object that adds 3 to any parameter:

```
add3(y) = add(3)
```

We can then use the `add3` function as a new function. We can get the results as follows:

```
add(10, 5) = 15
add3(10) = 13
add(2, 0) = 2
add4(y) = add(4)
add4(6) = 10
```

### λ-Expressions in C++

One advantage of C++ λ-expressions is they allow us to create functions with fewer lines of code.  Traditionally, we would create a function/method definition with parameters and return type. In C++, we can simplify this. For example, we can create an `add` function:

```cpp
auto add = [](int x , int y) { return x + y; };
```

The `[]` block allows parameter passing into the function. We define the parameters we want to pass to the function. Finally, in the curly brackets, we define the λ expression. We can use the `add` as a normal function:

```cpp
auto x = add(10, 12);
```

### Example Application

We will look at a collection of different ways to use λ-expressions in C++.  We are brushing the surface of λ-expressions here, but it is enough for what
we need.

#### Simple λ-Expression Example

We have already looked at the add example:

```cpp
// Create lambda expression
auto add = [](int x , int y){ return x + y; };
// Call the defined function
auto x = add (10 , 12);
// Display answer - should be 22
cout << "10 + 12 = " << x << endl;
```

This is the simplest form of λ-expression in C++. We use `auto` so the compiler will work out the type.

#### Function Objects

Another method is to create a `function` object.  For this, we need to include the `functional` header:

```cpp
#include <functional>
```

We can then create a `function` object:

```cpp
// Create function object with same lambda expression
function<int(int, int)> add_function = [](int x, int y){ return x + y; };
// Call the function object
x = add_function(20, 12);
// Display the answer - should be 32
cout << "20 + 12 = " << x << endl;
```

This is the same add example, except we are defining the type of the `function`.

#### Fixed Values

We already mentioned that we can pass fixed values within the square brackets `[ ]`:

```cpp
int a = 5;
int b = 10;
// Define the values passed to the function
auto add_fixed = [a, b]{ return a + b; };
// Call the function
x = add_fixed();
// Display the answer - should be 15
cout << "5 + 10 = " << x << endl;
```

If we change the values of `a` and `b`, the output of `add_fixed` does not change. So, if we do the following:

```cpp
// Change the values of a and b
a = 20; b = 30;
// Call the fixed function again
x = add_fixed();
// Display the answer - will come out as 15
cout << " 20 + 30 = " << x << endl;
```

The output of `add_fixed` will still be 15, not 50.

#### Reference Values

Although not a good practice, we can pass values to the function as references.  We can change the function definition:

```cpp
// Define the values passed to the function, but pass as a reference
auto add_reference = [&a, &b]{ return a + b; };
// Call the function
x = add_reference();
// Display the answer - should be 50
cout << "20 + 30 = " << x << endl;
// Change values of a and b
a = 30;
b = 5;
// Call the reference based function again
x = add_reference();
// Display the answer - should be 35
cout << "30 + 5 = " << x << endl;
```

Note the use of `&` to denote we are passing by reference. When we change the values of `a` and `b` now the output of the function is also changed.

#### Complete Application

The complete λ-expression application is given below:

```cpp
#include <iostream>
#include <functional>

using namespace std;

int main (int argc, char **argv)
{
    // Create lambda expression
    auto add = [](int x, int y) { return x + y; };
    // Call the defined function
    auto x = add(10 , 12);
    // Display answer - should be 22
    cout << "10 + 12 = " << x << endl;

    // Create function object with same lambda expression
    function<int(int, int)> add_function = [](int x, int y) { return x + y; };
    // Call the function object
    x = add_function(20, 12);
    // Display the answer - should be 32
    cout << "20 + 12 = " << x << endl;

    int a = 5;
    int b = 10;
    // Define the values passed to the function
    auto add_fixed = [a, b] { return a + b; };
    // Call the function
    x = add_fixed();
    // Display the answer - should be 15
    cout << "5 + 10 = " << x << endl;

    // Change values of a and b
    a = 20;
    b = 30;
    // Call the fixed function again
    x = add_fixed();
    // Display the answer - will come out as 15
    cout << "20 + 30 = " << x << endl;

    // Define the values passed to the function , but pass as reference
    auto add_reference = [&a, &b] { return a + b; };
    // Call the function
    x = add_reference();
    // Display the answer - should be 50
    cout << "20 + 30 = " << x << endl;

    // Change the values of a and b
    a = 30;
    b = 5;
    // Call the reference based function again
    x = add_reference();
    // Display the answer - should be 35
    cout << "30 + 5 = " << x << endl;

    return 0;
}
```

The output is:

```shell
10 + 12 = 22
20 + 12 = 32
5 + 10 = 15
20 + 30 = 15
20 + 30 = 50
30 + 5 = 35
```
