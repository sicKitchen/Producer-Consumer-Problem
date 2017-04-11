/******************************* 
 * Producer & Consumer problem *
 *  -Solved using Pthreads     *
 *                             *
 * Author: Spencer Kitchen     *
 * Date: 2/23/2017             *
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>        // For threads and mutex
#include <semaphore.h>      // For empty and full
#include <stdbool.h>        // For boolean values
#include <unistd.h>         // for sleep
#include <signal.h>         // to catch ctrl+c
#include "buffer.h"

#define MY_RAND_MAX 99      // Highest integer for random number generator
#define DEBUG false     // Turn on to see debug output
// Turn on if you want to see buffer printed in Producer and consumer threads   
#define PRINT_BUFFER false     

void *Producer_function( void *ptr);
void *Consumer_function( void *ptr);
int getRandomNumber(int MAX);
void printBuffer(buffer_item buffer[BUFFER_SIZE]);
void sig_handler(int signo);

// Create Mutex, empty, full
pthread_mutex_t mutex;   // only one can access buffer at a time
sem_t empty;             // empty slots
sem_t full;              // full slots

// Create the buffer 
buffer_item buffer[BUFFER_SIZE];

// Create flag for stoping threads, at first threads are active
bool active = true;

/*******************************************************
 * MAIN                                                *
 *  3 parameters from comman line                      *
 *  argv[1] = How long to sleep for before terminating *
 *  argv[2] = The number of producer threads           *
 *  argv[3] = The number of consumer threads           * 
 *******************************************************/ 
int main(int argc, char**argv) { 
    // Seed random number generator
    srand(time(NULL));

    // Check if we got the arguments
    int sleepNumber;
    int numberOfProducers;
    int numberOfConsumers;
    switch (argc){
        case 4:
            // Get and process command line arguments
            sleepNumber = atoi(argv[1]);
            numberOfProducers = atoi(argv[2]);
            numberOfConsumers = atoi(argv[3]);
            printf("Timeout number: %d\tNum of producers: %d\tNum of consumers: %d\n",
                sleepNumber, numberOfProducers, numberOfConsumers);
            break;

        default:
            printf("USAGE: enter sleep time, producer #, consumer #\n");
            exit (0);
            break;
    }

    // Register signal with kernal
    if(signal(SIGINT, sig_handler) == SIG_ERR){
        printf("\n cant catch SIGINT\n");
    }

    // Initialize empty - start at buffer size and count down 
    if (sem_init(&empty, 0, BUFFER_SIZE) != 0) {
        fprintf(stderr, "failed to initialize empty's\n");
    }

    // Initialize mutex 
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "failed to initialize mutex\n");
    }

    // Initialize full - start at 0 and count up  
    if (sem_init(&full, 0, 0) != 0) {
        fprintf(stderr, "failed to initialize full\n");
    }

    // new threads to hold producer & consumer 
    pthread_t producer_threads[numberOfProducers];
    pthread_t consumer_threads[numberOfConsumers];

    // Counts the number of producers/consumers
    int producerCount[numberOfProducers];
    int consumerCount[numberOfConsumers];

    // Create number list for producer threads
    for (int i = 1; i <= numberOfProducers; ++i)
    {
        producerCount[i-1] = i;    
    }

    // Create number list for consumer threads
    for (int i = 1; i <= numberOfConsumers; ++i)
    {
        consumerCount[i-1] = i;
    }


    // Used to hold the return values of pthread_join for error checking
    int  iretP, iretC;

    /* Create independent threads each of which will execute function */
    for (int i = 0; i < numberOfProducers; ++i)
    {
        pthread_create(&producer_threads[i], NULL, Producer_function, (void*) &producerCount[i]);
        printf("Creating producer thread: %d\n", i );
    }

    for (int i = 0; i < numberOfConsumers; ++i)
    {
        pthread_create(&consumer_threads[i], NULL, Consumer_function, (void*) &consumerCount[i]);
        printf("Creating consumer thread: %d\n", i );
    }

    sleep(sleepNumber);
    printf("sleep is over \n");
    // Want to terminate once passed in sleep time is over, turn off active flag
    active = false;

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    int count = 0;
    while(count < numberOfConsumers){
        iretC = pthread_join(consumer_threads[count], NULL);
        printf("Consumer thread %d returns: %d\n", count, iretC);
        count++;
    }
    count = 0;
    
    while(count < numberOfProducers){
        iretP = pthread_join(producer_threads[count], NULL);
        printf("Producer thread %d returns: %d\n", count, iretP);
        count++;
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    exit(0);
}

/*********************************************************************
 * Producer Thread                                                   *
 *  runs producer function that will generate a random integer       *
 *  between 1 and MY_RAND_MAX. Set MY_RAND_MAX value at top of code. *
 *  After it adds the number to the shared buffer if there is room   *
 *  for the number in the buffer. Finally sleeps for random time     *
 *  before trying to produce another integer.                        *
 *********************************************************************/
void *Producer_function( void *ptr) {
    int threadNumber = *((int *)ptr);

    do {
        // produce an item in next produced 
        int nextProduced = getRandomNumber(MY_RAND_MAX);
        if (DEBUG) printf("P: random number %d\n", nextProduced );

        // Now we see if there is room in the buffer
        //  by checking if the semaphore is open 
        
        if (DEBUG){
            // Check what value of empty semaphore is
            int empty_value; 
            sem_getvalue(&empty, &empty_value); 
            printf("P: The value of the empty semaphores is %d\n", empty_value);
        }

        // If empty != 0 then decrease and we move on, if not block
        if (active ){
            sem_wait(&empty);
        }else return NULL;
          
        // protect buffer, will wait here if already locked
        pthread_mutex_lock(&mutex);
        if(DEBUG) printf("P: I have lock on buffer\n");

        int full_value; 
        sem_getvalue(&full, &full_value); 
        if(DEBUG) printf("P: The value of the full semaphores is %d\n", full_value);

        buffer[full_value] = nextProduced;

        /* SPECIFIED OUTPUT FROM ASSIGNMENT */
        if (active) printf("producer %d produced %d\n", threadNumber , nextProduced);
    
        // Print contents of buffer after new int is added
        if(PRINT_BUFFER) printBuffer(buffer);
        // Unlock the buffer
        pthread_mutex_unlock(&mutex);
        if(DEBUG) printf("P: I have unlocked on buffer\n");

        // increment full
        if (active) sem_post(&full);

        // Sleep for 1-5 seconds
        sleep(getRandomNumber(5));


    } while (active);

    // Will wait if empty equals zero, which is likely to happen if producers 
    //  produce faster then consumers consume. To clean up and stop deadlock
    //  increment empty when threads terminates
    sem_post(&empty);
    return NULL;
}

/************************************************************************ 
 * Consumer Thread                                                      *
 *  Looks at buffer and pulls the last produced integer from producer   *
 *  thread. It will print the integer it pulled from buffer out. After, *
 *  thread will sleep for random time before trying to pull another     *
 *  number from the buffer.                                             *
 ************************************************************************/
void *Consumer_function( void *ptr){
    int threadNumber = *((int *)ptr);

    do
    {
        int nextConsumed;      // Holds consumed value

        if (DEBUG)
        {
            int empty_value; 
            sem_getvalue(&empty, &empty_value); 
            printf("C: The value of the empty semaphores is %d\n", empty_value);
        }

        // Check if full != 0, if it is block
        if (active){
            sem_wait(&full);
        } else return NULL;


        // Get lock on buffer
        pthread_mutex_lock(&mutex);
        if(DEBUG) printf("C: I have lock on buffer\n");

        int full_value; 
        sem_getvalue(&full, &full_value); 
        if(DEBUG) printf("C: The value of the full semaphores is %d\n", full_value);

        // remove item from buffer
        nextConsumed = buffer[full_value];
        // Set buffer to 0 since we consumed that integer
        buffer[full_value] = 0;

        /* SPECIFIED OUTPUT FROM ASSIGNMENT */
        if (active) printf("consumer %d consumed %d\n", threadNumber ,nextConsumed);
        
        // Print the buffer after consumer consumed integer
        if(PRINT_BUFFER) printBuffer(buffer);

        pthread_mutex_unlock(&mutex);
        if(DEBUG) printf("C: I have unlocked on buffer\n");

        // increase empty 
        if (active) sem_post(&empty);

        // Sleep for 1-5 seconds
        sleep(getRandomNumber(5));

    } while (active);

    // Will wait if full equals zero, which is likely to happen if consumers 
    //  consumes faster then producers produce. To clean up and stop deadlock
    //  increment full when threads terminates
    sem_post(&full);
    return NULL;
}

/******************************************
 * Simple fuction to print out the buffer *
 ******************************************/
void printBuffer(buffer_item buffer[BUFFER_SIZE]){
     for (int i = 0; i < BUFFER_SIZE; ++i)
     {
          printf("%d ", buffer[i]);
     }
     printf("\n");
}

/************************************************
 * Generates a random integer between 1 and MAX *
 ************************************************/
int getRandomNumber(int MAX){
     int n;
     n = (rand() % MAX) + 1;
     return n;
}

/***************************************************
 * Signal Handler                                  *
 *  is called when ctrl + c is pressed on keyboard *
 ***************************************************/
 void sig_handler(int signo) {
     if (signo == SIGINT){
         printf("received SIGINT\n");
         // Turn off threads
         active = false;
     }

 }


