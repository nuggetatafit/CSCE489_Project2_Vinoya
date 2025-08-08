#include <pthread.h>
#include "Semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


/*************************************************************************************
 * Semaphore (constructor) - this should take count and place it into a local variable.
 *						Here you can do any other initialization you may need.
 *
 *    Params:  count - initialization count for the semaphore
 *
 *************************************************************************************/

Semaphore::Semaphore(int count) {

    this->count = count; // pointer for count

    if (pthread_mutex_init(&mutex, NULL) != 0) { // condition for mutex initialization if failed
        fprintf(stderr, "Failed to initialize mutex\n"); // print error message
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_init(&condition, NULL) != 0) { // condition for condition variable initialization if failed
        fprintf(stderr, "Failed to initialize condition variable\n"); // print error message
        exit(EXIT_FAILURE);
    }
}

/*************************************************************************************
 * ~Semaphore (destructor) - called when the class is destroyed. Clean up any dynamic
 *						memory.
 *
 *************************************************************************************/

Semaphore::~Semaphore() {
    // clean up the mutex and condition variable
    pthread_mutex_destroy(&mutex); // destroy the mutex
    pthread_cond_destroy(&condition); // destroy the condition variable
    
    // no dynamic memory to free in this implementation
    // if you had allocated memory, you would free it here
}

/*************************************************************************************
 * wait - implement a standard wait Semaphore method here
 *
 *************************************************************************************/

void Semaphore::wait() {

    pthread_mutex_lock(&mutex); // lock the mutex

    while (count <= 0) { // wait until count is greater than 0
        pthread_cond_wait(&condition, &mutex); // wait on the condition variable
    }

    count--; // decrement the count
    pthread_mutex_unlock(&mutex); // unlock the mutex

}

/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() {

    pthread_mutex_lock(&mutex); // lock the mutex
    count++; // increment the count
    pthread_cond_signal(&condition); // signal one waiting thread, if any
    pthread_mutex_unlock(&mutex); // unlock the mutex

}