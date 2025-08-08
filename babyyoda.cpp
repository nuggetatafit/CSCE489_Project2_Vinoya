/*************************************************************************************
 * babyyoda - used to test your semaphore implementation and can be a starting point for
 *			     your store front implementation
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Semaphore.h"
#include <vector> 

// Semaphores that each thread will have access to as they are global in shared memory
Semaphore *empty = NULL;
Semaphore *full = NULL;

// global variables for the buffer and mutex
pthread_mutex_t mutex; // mutex to protect buffer access
std::vector<int> buffer; // this is our new buffer as a vector
// this is basically the same thing as buffer_item buffer[BUFFER_SIZE] from the book
// but we can put any size instead

int consumed = 0; // how many items have been consumed
int buffer_size = 0; // size of the buffer/shelf
int max_items = 0; // max items to produce

int next_item_produced = 0; // index for the next item to be produced
int next_item_consumed = 0; // index for the next item to be consumed

/*************************************************************************************
 * producer_routine - this function is called when the producer thread is created.
 *
 *			Params: data - a void pointer that should point to an integer that indicates
 *							   the total number to be produced
 *
 *			Returns: always NULL
 *
 *************************************************************************************/
 
 // this function will produce items and place them into the buffer
void *producer_routine(void *data) { // data is a void pointer that should point to an integer
	time_t rand_seed; // seed for random number generation
	srand((unsigned int) time(&rand_seed)); // seed the random number generator

	// The current serial number (incremented)
	unsigned int serialnum = 1;
	
	// We know the data pointer is an integer that indicates the number to produce
	int left_to_produce = *((int *) data);

	// loop through the amount we're going to produce and place into the buffer
	while (left_to_produce > 0) {
		printf("Producer wants to put Yoda #%d into buffer...\n", serialnum);

		// Semaphore check to make sure there is an available slot
		full->wait();

		// Place item on the next shelf slot by first setting the mutex to protect our buffer vars
		pthread_mutex_lock(&mutex);

		buffer[next_item_produced] = serialnum; // make this circular buffer; add item in
		next_item_produced = (next_item_produced + 1) % buffer.size(); // move to next slot
		left_to_produce--; // decrement the number left to produce

		pthread_mutex_unlock(&mutex); // unlock the mutex
		printf("   Yoda #%d put on shelf.\n", serialnum); // message for item placed on shelf

		serialnum++; // increment serial number
		
		// Semaphore signal that there are items available
		empty->signal();

		// random sleep but he makes them fast so 1/20 of a second
		usleep((useconds_t) (rand() % 200000));
	
	}
	return NULL; // return NULL to indicate thread completion
}

/*************************************************************************************
 * consumer_routine - this function is called when the consumer thread is created.
 *
 *       Params: data - a void pointer that should point to a boolean that indicates
 *                      the thread should exit. Doesn't work so don't worry about it
 *
 *       Returns: always NULL
 *
 *************************************************************************************/

// this function will consume items from the buffer
void *consumer_routine(void *data) {
	long thread_id = (long) data; // get the thread ID from the data pointer

	while (true) { // loop until we exit
		printf("Consumer #%ld wants to buy a Yoda...\n", thread_id);

		// Semaphore to see if there are any items to take
		empty->wait();

		// Take an item off the shelf
		pthread_mutex_lock(&mutex);

		if (consumed == max_items) { // check if we have consumed enough items
			pthread_mutex_unlock(&mutex); // unlock the mutex before exiting
			empty->signal(); // signal that there are no items available
			printf("Consumer #%ld found no Yoda to buy, going home.\n", thread_id);
			break; // exit
		}

		int item = buffer[next_item_consumed]; // get the item from the buffer
		next_item_consumed = (next_item_consumed + 1) % buffer.size(); // move to next slot
		consumed++;	// increment the number of items consumed

		
		printf("   Consumer #%ld bought Yoda #%d.\n", thread_id, item); // print out the item that was consumed
		pthread_mutex_unlock(&mutex); // print out how many items have been consumed

		// Consumers wait up to one second
		usleep((useconds_t) (rand() % 1000000));
		full->signal(); // signal that there is an item available

	}
	printf("Consumer #%ld goes home.\n", thread_id); // message for consumer exiting
	return NULL; // return NULL to indicate thread completion
}

/*************************************************************************************
 * main - Standard C main function for our storefront. 
 *
 *		Expected params: pctest <buffer_size> <num_consumers> <max_items>
 *				max_items - how many items will be produced before the shopkeeper closes
 *				buffer_size - how many items can be on the shelf at once
 *				num_consumers - how many consumers will be shopping today
 *
 *************************************************************************************/

// global variables for buffer size, number of consumers, and max items to produce
int main(int argv, const char *argc[]) {

	// Get our argument parameters (I adjusted this for buffer_size and num_consumers as well)
	if (argv < 4) { // check if we have enough parameters (should be 4, ./babyyoda <buffer_size> <num_consumers> <max_items>)
		printf("Invalid parameters. Format: %s <buffer_size> <num_consumers> <max_items>\n", argc[0]); // print error message
		exit(0);
	}

	// user input for buffer size, number of consumers, and max items to produce
	buffer_size = (unsigned int) strtol(argc[1], NULL, 10); // input buffer size
	buffer.resize(buffer_size); // resize the buffer to the specified size based on vector
	int num_consumers = (unsigned int) strtol(argc[2], NULL, 10); // input number of consumers
	max_items = (unsigned int) strtol(argc[3], NULL, 10); // input max items to produce

	// check if the input values are valid (positive integers)
	if (buffer_size < 0 || num_consumers < 0 || max_items < 0) {
		printf("Invalid parameter input. Must be a positive integers.\n"); // print error message
		exit(0); // exit if invalid buffer size
	}

	// print messages
	printf("Producing %d today.\n", max_items); // message for max items
	printf("Holding %d items on the shelf.\n", buffer_size); // message for buffer size
	printf("There are %d consumers shopping today.\n", num_consumers); // message for number of consumers
	printf("The store will close after %d items are sold.\n", max_items); // message for max items to produce
	
	// Initialize our semaphores and buffer mutex
	empty = new Semaphore(0); // initially no items in the buffer
	full = new Semaphore(buffer_size); // initially the buffer is full
	pthread_mutex_init(&mutex, NULL); // initialize our buffer mutex

	// Create our producer threads
	pthread_t producer; // producer thread
	std::vector<pthread_t> consumers(num_consumers); // vector array of consumer threads
	pthread_create(&producer, NULL, producer_routine, (void *) &max_items); // launch producer threads

	// launch consumer threads
	for (long i=0; i<num_consumers; i++) { // loop through the number of consumers
		pthread_create(&consumers[i], NULL, consumer_routine, (void *) i); // create consumer thread
	}

	// Wait for our producer thread to finish up
	pthread_join(producer, NULL); // wait for producer to finish
	printf("The manufacturer has completed his work for the day.\n");
	printf("Waiting for consumers to buy up the rest.\n");

	// Give the consumers a second to finish snatching up items
	while (consumed < buffer_size)
		sleep(1); // wait for consumers to finish consuming

	empty->signal(); // signal that there are no items available

	// Now signal all consumers to exit and make sure they exited
	for (int i=0; i<num_consumers; i++) { // loop through the number of consumers
	//	empty->signal(); // signal that there are no items available
		pthread_join(consumers[i], NULL); // wait for each consumer to finish
	}

	// we are exiting, clean up; delete semaphores
	delete empty;
	delete full;
	printf("Producer/Consumer simulation complete!\n");
}
