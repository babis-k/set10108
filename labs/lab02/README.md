# Lab 02: Multi-threading exercises

The aim of the second lab is to make you think about the use of thread pools and pipelines. It will consist of two exercises, and you will have an hour for each to achieve as much as you can. For each of the two exercises, some skeleton code will be provided, that uses some constructs that you will see next time, so for now they are given.

# Exercise 1: Thread pool

Tasks: 
        
* Initialize a thread pool as a vector of running threads. Make sure the application exits gracefully if we input "stop" and exit the loop
	* The thread pool contains two types of tasks. It's up to you to decide how many threads to devote to each type of task, and how big the thread pool is.
        
* Based on the contents of the string, call a different task:
	* If the word is long (> 4 letters), run a task called variable_processor that sleeps for text.length msec
    * If the word is short (<= 4 letters), run a task called fixed_processor that sleeps for 2 msec

* Try it first with input given from the user:
	* when user inputs string, print the thread id and task type  (e.g. variable_processor or fixed_processor)
        
* Try it using the lorem ipsum text tokens, and experiment and find what works best for your system: 
	* how big should your thread pool be before you see penalties?
	* what percentage of threads should you devote for each type of task?

* Add another task that always runs (in addition to the other two) and calculates the frequencies of each word in the lorem ipsum text tokens, and prints them just before it joins

# Exercise 2: Pipeline

* Task 1: Create the threads & code the logic for the do_work function:
	* Create and run the threads for each of wash/dry/iron (1 thread each)
	* make sure the threads will be running while there's still work to do (what condition? How frequently to check the condition is up to you)
	* write the do_work function: while we still have potential work to do, Pop a load from the pile, work on it (sleep for the work duration) and add it to the output pile

* Task 2: Measure performance for 1000 loads

* Task 3: Investigate best thread distribution. Have N threads for washing, M threads for drying, P threads for ironing. Which numbers get you the best performance?
	* a brute force approach: instead of putting your code in the main function, put it in another function, which you can call with parameters


