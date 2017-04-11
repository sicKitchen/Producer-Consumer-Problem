# Write-up

## Implementation Details

* Use standard counting semaphores for `empty` and `full` and a mutex lock to represent `mutex`.
* Solve using Linux Pthread API

### The Buffer

* `buffer` will be a fixed array of type buffer_item and is defined as

```C
/* buffer.h */
typedef int buffer item;
#define BUFFER SIZE 5
```

* Buffer will be manipulated by two types of functions, `Producer_function()` and `Consumer_function()`.

### Main Function

* Will initialize the buffer and create the separate producer and consumer threads.
* Once it has created the threads the main  function will sleep for a period of time
* On awakening, the main thread will gracefully terminate the application.
* Takes in 3 parameters from command line
    1. How long to sleep before terminating
    1. The number of producer threads
    1. The number of consumer threads
* If a SIGINT is received before the sleep timer has ran out, the program will gracefully shut down running
  threads and terminate early.

### Producer & Consumer Threads

* The producer thread will alternate between sleeping for a random period of time and inserting a random integer into the buffer. Random numbers will be produced using the rand() function, which produces random integers between 0 and RAND MAX.

```c
//Structure for producer process

  do {
        ...
    /* produce an item in next produced */
        ...
    wait(empty);
    wait(mutex);
        ...
    /* add next produced to the buffer */
        ...
    signal(mutex);
    signal(full);
 } while (true);
```

* The consumer will also sleep for a random period of time and, upon awakening, will attempt to remove an item from the buffer.

```c
//Structure for Consumer process

do {
    wait(full);
    wait(mutex);
        ...
    /* remove an item from buffer to next consumed */
        ...
    signal(mutex);
    signal(empty);
        ...
    /* consume the item in next consumed */
        ...
} while (true);
```

## Sample output

### Test 1

Test 1 will create one producer and one consumer thread. Program will terminate in 10 seconds.

TO RUN:

```txt
 ./producer_consumer 10 1 1
```

**OUTPUT**

```txt
Timeout number: 10    Num of producers: 1    Num of consumers: 1
Creating producer thread: 0
Creating consumer thread: 0
producer 1 produced 5
consumer 1 consumed 5
producer 1 produced 38
consumer 1 consumed 38
producer 1 produced 68
consumer 1 consumed 68
producer 1 produced 63
sleep is over
Consumer thread 0 returns: 0
Producer thread 0 returns: 0
```

### Test 2

Test 2 will create 5 producer and 3 consumer thread. Program will terminate in 10 seconds.

TO RUN:

```txt
 ./producer_consumer 10 5 3
```

**OUTPUT**

```txt
Timeout number: 10    Num of producers: 5    Num of consumers: 3
Creating producer thread: 0
Creating producer thread: 1
producer 1 produced 44
Creating producer thread: 2
producer 2 produced 3
producer 3 produced 38
Creating producer thread: 3
producer 4 produced 28
Creating producer thread: 4
producer 5 produced 28
Creating consumer thread: 0
consumer 1 consumed 28
Creating consumer thread: 1
consumer 2 consumed 28
Creating consumer thread: 2
consumer 3 consumed 38
producer 4 produced 85
consumer 2 consumed 85
producer 3 produced 55
consumer 3 consumed 55
producer 4 produced 70
producer 2 produced 61
consumer 1 consumed 61
consumer 2 consumed 70
producer 1 produced 55
producer 5 produced 1
producer 2 produced 23
consumer 3 consumed 23
consumer 1 consumed 1
producer 1 produced 47
producer 3 produced 61
consumer 3 consumed 61
consumer 1 consumed 55
producer 4 produced 88
producer 2 produced 23
consumer 2 consumed 0
producer 5 produced 12
sleep is over
Consumer thread 0 returns: 0
Consumer thread 1 returns: 0
Consumer thread 2 returns: 0
Producer thread 0 returns: 0
Producer thread 1 returns: 0
Producer thread 2 returns: 0
Producer thread 3 returns: 0
Producer thread 4 returns: 0
```

### Test 3

Test 3 will create 3 producer and 5 consumer thread. Program will terminate in 10 seconds.

TO RUN:

```txt
 ./producer_consumer 10 3 5
```

**OUTPUT**

```txt
Timeout number: 10    Num of producers: 3    Num of consumers: 5
Creating producer thread: 0
Creating producer thread: 1
Creating producer thread: 2
producer 2 produced 95
Creating consumer thread: 0
producer 1 produced 43
Creating consumer thread: 1
Creating consumer thread: 2
producer 3 produced 85
Creating consumer thread: 3
consumer 1 consumed 43
Creating consumer thread: 4
consumer 3 consumed 95
consumer 2 consumed 0
producer 3 produced 26
consumer 3 consumed 26
producer 1 produced 29
consumer 5 consumed 29
producer 2 produced 67
consumer 4 consumed 67
producer 1 produced 10
producer 3 produced 66
consumer 2 consumed 66
consumer 1 consumed 10
producer 1 produced 95
consumer 3 consumed 95
producer 2 produced 13
consumer 4 consumed 13
sleep is over
Consumer thread 0 returns: 0
Consumer thread 1 returns: 0
Consumer thread 2 returns: 0
Consumer thread 3 returns: 0
Consumer thread 4 returns: 0
Producer thread 0 returns: 0
Producer thread 1 returns: 0
Producer thread 2 returns: 0
```

#### Output Description

In all test cases, the producers will keep producing threads as long as the buffer is not full
and the consumer threads will keep consuming threads until the buffer is empty. When the sleep time
is over the main will wait for all the consumer and producer threads to join back up with the main function
before exiting.

## Authors

* **Spencer Kitchen**
* **Email:** spencer.kitchen@wsu.edu

