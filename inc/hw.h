///
/// @file hw.h
/// @author Ayodeji Raymond
///

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <string>

/// @brief This is a simple header file that contains 5 functions and an enum.
/// The enum thread_type helps to differentiate between READER and WRITER 
/// threads in a way thats easy to understand. The functions reader(void *arg) 
/// and writer(void *arg) are the functions linked to the reader and writer 
/// threads. run_rw_problem(int num_readers, int num_writers, bool priority)
/// allows a user to manipulate the thread functionality with varying number of 
/// reader threads, writer threads and set the reader_priority. 
/// test_access_vector() tests wether or not the READER threads are able to 
/// access the shared data simultneously and if WRITER threads are the exclusive 
/// accessors of the data when they write. test_order_vect() verifies that the 
/// code obeys the priority specifications
/// @author Ayodeji Raymond
///

enum thread_type
{
    READER,
    WRITER
};

void *reader(void *arg);
void *writer(void *arg);
void run_rw_problem(int num_readers, int num_writers, bool priority);
bool test_access_vector();
std::string test_order_vector();

