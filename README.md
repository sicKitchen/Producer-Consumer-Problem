# Producer - Consumer Problem

Due: 2/26
CS 460 Project 2

This is an implementation of the Producer - Consumer problem using semaphores and a mutex.

## Producer Consumer Problem

In computing, the producer–consumer problem (also known as the bounded-buffer problem) is a classic example of a multi-process synchronization problem. The problem describes two processes, the producer and the consumer, who share a common, fixed-size buffer used as a queue. The producer's job is to generate data, put it into the buffer, and start again. At the same time, the consumer is consuming the data (i.e., removing it from the buffer), one piece at a time. The problem is to make sure that the producer won't try to add data into the buffer if it's full and that the consumer won't try to remove data from an empty buffer.

[wiki](https://en.m.wikipedia.org/wiki/Producer–consumer_problem)

### Additional Requirements

Install a handler to intercept SIGINT signal (ctrl+c). In this handle set a flag to tell the producer and consumer threads to terminate itself. The main program will exit once both the producer and the consumer have terminated themselves.

## Prerequisites

Code is only tested for linux, specifically Ubuntu 16.04

## Installing

* First extract Cs460-project2-SpencerKitchen-11480841.tar.gz,
  you will see a folder called `producer_consumer_problem`

* Go to folder

  `cd ~/.../producer_consumer_problem`

* From folder type `make` to compile program

  You should get an executable called `producer_consumer`

* To run program type `./producer_consumer` followed by 3 arguments

  Sample Input:

```bash
./producer_consumer 30 1 1
```

* To compile from scratch

```bash
make clean
make
```

* NOTE: See `writeup.md` for test input and program specifics.

## Built With

* C
* Pthreads

## Authors

* **Spencer Kitchen**
* **Email:** spencer.kitchen@wsu.edu - sckitchen.dev@gmail.com

## Acknowledgments

* [Pthread Help](https://computing.llnl.gov/tutorials/pthreads/)
* [Signal Help](http://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/)


