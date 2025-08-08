# AFIT-CSCE489-PROJ2 - Threads/Semaphores

![AFIT](https://www.afit.edu/images/seals/fp_ENG_w.png)

**By 2d Lt Nathaniel Vinoya**

**CSCE489 - Project #2**

**08 August 2025**

------------------

## Introduction

The purpose of this project was to get introduced to multithreaded programming in the Linux environment by exploring the concept of process synchronization and solving the producer-consumer problem. Within this repository, we are implementing semaphores using monitor functions from the `<pthreads>` library.

In our scenario, there is a single producer that creates baby yoda toys and multiple consumers who are buying these toys. After the producer creates the toys, it puts it on a specified number of shelve spaces (buffer), where the consumer takes the toys. This scenario addresses the challenges of coordinating multiple threads that share a limited resource (buffer), and we are mainly looking to prevent race conditions of buffer overflow/underflow.

The files of interest include the following:

1. `babyyoda.cpp` - This is the main program that simulates the toy store scenario, ultimately creating the producer and consumer threads, determining the parameters (buffer size, maximum amount of items, and number of consumers), and dictating how they interact with the shelf/buffer space.

2. `Semaphore.h` - This is the header file that declares the structure and public interface of the semaphore.

3. `Semaphore.cpp` - This is the custom semaphore implementation class that has the constructor, destructor, wait, and signal methods.

------------------

## Components of `Semaphore.cpp` / `Semaphore.h`

This `Semaphore.cpp` class includes:

1. `Semaphore(int count)` - This constructor takes count, places it into a local variable, and initializes the semaphore.

2. `~Semaphore()` - This destructor destroys and semaphore and deallocates any dynamic data.

3. `void wait()` - This implements the `wait` semaphore method.

4. `void signal()` - This implements the `signal` semaphore method.

The header file includes the public interface and guarded buffer management variables, in particular the mutex lock, the condition variable, and the internal count value.

Overall, these two files provide the basic functionality.

------------------

## Components of `babyyoda.cpp`

This class includes:

1. `producer_routine` - This function is called when the producer thread is created.

2. `consumer_routine` - This function is called when the consumer thread is created.

3. `main` - Standard C main function for our storefront. 

------------------

## How to Use

You can use this repository by doing the following:

1. Download the contents within this GitHub repository.
2. Type `cd ./CSCE489_Project2`
3. Compile the code using `make clean; make`
4. Run `./babyyoda <buffer_size> <num_consumers> <max_items>`

`<buffer_size>` is the maximum number of items that can be stocked at once on the shelves/buffers.

`<num_consumers>` is the number of consumer threads in the store.

`<max_items>` is the number of items that that producer should stock on the shelves before they retire.

All of these need to be positive numbers.

------------------

## Limitations

This current implementation is only used for a single-producer, multi-consumer scenario, but is not designed for handing multiple-producers, making it limited in its design-versatility to handle different race condition problems. In order to adjust it to handle this scenario, we would need to ensure that the multiple producers would not crate items with duplicate serial numbers.

------------------

## Question and Answer

*Assuming the shopkeeper hires another worker, what changes would you need to make to turn this into a multi-producer, multi-consumer solution? I'm not asking you to write the code, just provide general pseudocode answers (10 pts)*

If there are more producers in this scenario, then we would need to ensure that each item produced has its own unique serial number and all of the producers stop at the `max_items` limit. Ultimately, the two main parts of this project that we would need to adjust are the `producer_routine` method and the `main` method. 

For the `producer_routine`, we would need to adjust the code so that there is a new mutex lock to acquire a unique serial number for the producers. There would also be a shared counter `need_to_produce` for the producers to coordinate production, so that they cumulatively reach the max_items. Similar to the `consumer_routine`, the producers would loop through its own `left_to_produce` count to decrease the `need_to_produce` counter to distribute productions amongst all of the producers. The following pseudocode is similar to the `consumer_rountine` code:

`max_items = need_to_produce`
`initialize producer_mutex`
`while (true) {`
    `lock producer_mutex // to prevent race conditions`
    `if need_to_produce = 0 // if there is no more`
        `unlock the producer_mutex`
        `break`
    `// if there is more, continue decrementing the count`
    `need_to_produce = need_to_produce - 1`
    `unlock producer_mutex`
    `...`
`}`

For the `main` function, we need to adjust it so that the producer threads are created and launched the same way the consumer ones are, by being looped through. We also need to adjust the prompt input to match to `./babyyoda <buffer_size> <num_producers> <num_consumers> <max_items>`.

`int num_producers = (unsigned int) strtol(argc[2], NULL, 10);`
`int num_consumers = (unsigned int) strtol(argc[3], NULL, 10);`
`max_items = (unsigned int) strtol(argc[4], NULL, 10);`
`...`
`for (int i=0; i<num_producers; i++)`
    `pthread_create(&producer_threads[i], NULL, producer_routine,...`
`...`
`for (int i=0; i<num_producers; i++)`
    `empty->signal();`
    `pthread_join(producer_threads[i], NULL);`

The rest of the code is adjusted to support the multiple producer threads; anything consumer thread related stays the same.

------------------

## Final Note

If you have any questions, comments, or concerns, feel free to contact me through the following means:

**AU Teams:** nathaniel.vinoya.1@au.af.edu

**.mil Email:** nathaniel.vinoya.1@us.af.mil

**AFIT Email:** nathaniel.vinoya@afit.edu

------------------

## References

[CSCE 489 Project #2 Prompt](https://drive.google.com/file/d/1iQoIfLmn8JDzwsvHwvIU7ENB0H4f8Fa-/view?usp=sharing)

[Operating System Concepts 9th Ed](https://drive.google.com/file/d/1AFRyycszmrdGeOywOkMrV1CxjNg0Qj7P/view?usp=sharing)

[Geeks for Geeks #1](https://www.geeksforgeeks.org/cpp/cpp-20-semaphore-header)
 
[Geeks for Geeks #2](https://www.geeksforgeeks.org/operating-systems/semaphores-in-process-synchronization/)

[Wikepedia (referenced the code)](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem)

[The Producer-Consumer Problem in C++](https://andrew128.github.io/ProducerConsumer/)