#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore 
{
public:

	Semaphore(int count);
	~Semaphore();

	void wait();
	void signal();

private:
	
	int count; // semaphore's internal value
	pthread_mutex_t mutex; // lock to protect count from race conditions
	pthread_cond_t condition; // condition variable to make threads wait and wake them up

};

#endif
