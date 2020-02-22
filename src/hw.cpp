///
/// @file hw.cpp
/// @author Ayodeji Raymond
///
#include "hw.h"

// semaphores
sem_t mx_x, mx_y,mx_z;
sem_t wsem,rsem;
sem_t order_mux;

// the shared data
int data = 0,

// read and write counts
readcount = 0, writecount = 0;

// reader priority indicator
bool reader_priority = false;

// vectors for testing purposes 
std::vector<std::pair<time_t,thread_type>> access_vect;
std::vector<thread_type> order_vect;

/// @brief this is the function for each reader thread
///
/// This function reads in the ID and stores it. The functionlity is different
/// depending on whether read_priority evaluates to true or false. If its true,
/// then READER threads have access priority. This means that they block until
/// they are all finished reading. Once done, the WRITER threads get their turn
/// If false, then WRITER threads have access priority and may block until
/// all writers are finished accessing. READER threads can access the data
/// simultaneously. 
///
/// @author Ayodeji Raymond
/// @param[in]   arg   the ID number (or index) of the thread
void *reader(void *arg)
{
    long id;
    id = ((long)arg);

    if (reader_priority)
    {
        // update readcount
        sem_wait(&mx_x);
        readcount +=1;
        if (readcount == 1)
            sem_wait(&wsem);    
        sem_post(&mx_x);

        // performs read
        printf("Data read by the reader%lu is %d\n",id,data);
    
        // addes thread type to order_vect
        sem_wait(&order_mux);
        order_vect.push_back(thread_type::READER);
        sem_post(&order_mux);

        // enters info into the access_vect    
        std::pair<time_t,thread_type> vect_elem(std::time(0),thread_type::READER);
        access_vect[id] = vect_elem; 

        // sleep so we know things work
        sleep(1);
 
        // update readcount 
        sem_wait(&mx_x);
        readcount -=1;
        if (readcount == 0)
            sem_post(&wsem);    
        sem_post(&mx_x);
    }
    else 
    {
        // waits until writer releases semaphore before proceeding
        sem_wait(&mx_z);
        sem_wait(&rsem);

        // updates readcount        
        sem_wait(&mx_x);
        readcount++;
        if (readcount == 1)
            sem_wait(&wsem);
        sem_post(&mx_x);

        sem_post(&rsem);
        sem_post(&mx_z);

        // perform reading
        printf("Data read by the reader%lu is %d\n",id,data);

        // addes thread type to order_vect
        sem_wait(&order_mux);
        order_vect.push_back(thread_type::READER);
        sem_post(&order_mux);

        // enters info into the access_vect    
        std::pair<time_t,thread_type> vect_elem(std::time(0),thread_type::READER);
        access_vect[id] = vect_elem;
        
        // sleep so we know things work
        sleep(1);

        // updates readcount
        sem_wait(&mx_x);
        readcount--;
        if (readcount == 0)
            sem_post(&wsem);
        sem_post(&mx_x); 
    }
    return 0;
}

/// @brief this is the function for each writer thread
///
/// This function reads in the ID and stores it. The functionlity is different
/// depending on whether read_priority evaluates to true or false. If its true,
/// then READER threads have access priority. This means that they block until
/// they are all finished reading. Once done, the WRITER threads get their turn
/// If false, then WRITER threads have access priority and may block until
/// all writers are finished accessing. WRITER threads need sole access to the
/// shared resource when they write. This means no READER threads can read and
/// no other WRITERS can write.
///
/// @author Ayodeji Raymond
/// @param[in]   arg   the ID number (or index) of the thread
void *writer(void *arg)
{
    long id;
    id = ((long) arg);

    if (reader_priority)
    {
        // wait until wsem is released by READERs
        sem_wait(&wsem);

        // perform writing
        data++;
        printf("Data writen by the writer%lu is %d\n",id,data);

        // addes thread type to order_vect
        sem_wait(&order_mux);
        order_vect.push_back(thread_type::WRITER);
        sem_post(&order_mux);
        
        // enters info into the access_vect    
        std::pair<time_t,thread_type> vect_elem(std::time(0),thread_type::WRITER);
        access_vect[id] = vect_elem; 
        
        // sleep so we know things work
        sleep(1);
        sem_post(&wsem);
    }
    else
    {
        // update writecount
        sem_wait(&mx_y);
        writecount++;
        if (writecount==1)
            sem_wait(&rsem);
        sem_post(&mx_y);
   
        // wait until rsem is released and in turn lock it so we block READER
        // thread access 
        sem_wait(&wsem);

        // perform writing 
        data++;
        printf("Data writen by the writer%lu is %d\n",id,data);

        // adds thread type to order_vect
        sem_wait(&order_mux);
        order_vect.push_back(thread_type::WRITER);
        sem_post(&order_mux);

        // enters info to the access_vect
        std::pair<time_t,thread_type> vect_elem(std::time(0),thread_type::WRITER);
        access_vect[id] = vect_elem; 

        // sleep so we know things work
        sleep(1);

        sem_post(&wsem);

        // update writecount
        sem_wait(&mx_y);
        writecount--;
        if (writecount == 0)
            sem_post(&rsem);
        sem_post(&mx_y);
    }
    return 0;
}

/// @brief this function tests whether READERS accessed simultaneously
/// and whether WRITERS blocked as they should
///
/// This function operates on the vector access_vect. access_vect contains pairs
/// or timestamps and thread_types. The vector is ordered at the end of 
/// run_rw_problem(). By noting when each thread accessed the shared resources
/// we can compare access times to determine if READERS accessed the shared
/// resource simultaneously and whether WRITERS blocked for a full second
/// before any other thread was able to access. This function verifies that
///
/// @author Ayodeji Raymond
/// @return Returns validity of the vector.
bool test_access_vector()
{
    bool valid = true; 

    // loops through all elements and compares access time of the current
    // element and the next element
    for(std::size_t i=0; i < access_vect.size(); i++)
    {
        if (i+1 < access_vect.size())
        {
            if (access_vect[i].second == thread_type::WRITER)
            {
                if (access_vect[i+1].first - access_vect[i].first < 1)
                {
                    valid = false;
                    break;
                }
            }
            else if (access_vect[i].second == thread_type::READER)
            {
                if (access_vect[i+1].first - access_vect[i].first >= 1)
                {
                    // special case where final reader actually sleeps the full
                    // second before releasing the lock
                    if (access_vect[i+1].second != thread_type::WRITER)
                    {
                        valid = false;
                        break;
                    }
                }
            }
        }
    }
    return valid;
}

/// @brief this function tests whether priorites are followed when multiple thread
/// access the shared resource
///
/// This function operates on the vector order_vect. order_vect contains
/// thread_types. 0 represents thread_type::READER and 1 represent
/// thread_type::WRITER. Each time a thread accesses the shared resource, it
/// also pushes its thread_type onto the vector. Based on the order, we can
/// determine if priority was followed. The function returns this vector
/// in string format to be compared to to the test cases.
///
/// @author Ayodeji Raymond
/// @return Returns an std::string that represents the binary contents of vector
std::string test_order_vector()
{
    std::string str = "";
    for (std::vector<thread_type>::const_iterator i = order_vect.begin(); i != order_vect.end(); ++i)
        str += std::to_string(*i);
    return str;
}

/// @brief this thread runs the solution to the reader writer problem
///
/// This function initializes semaphores. It then starts a number
/// of reader threads and writer threads and gives each a unique ID number.
/// Each thread runs the reader function or the writer function and then joins
/// upon completion. When accessing the critical zone, each thread writes
/// its access time to a vector at the index that corresponds to its ID number.
/// This vector is sorted. The purpose of this vector is to verify functionality 
/// for the test cases.
///
/// @author Ayodeji Raymond
/// @param[in]   num_readers   the number of reader threads 
/// @param[in]   num_writers   the number of writer threads
/// @param[in]   priority      determinating parameter between writer priority 
/// and reader priority
/// @return      Returns vector of pairs of timestamps and thread_types
void run_rw_problem(int num_readers, int num_writers, bool priority)
{
    reader_priority = priority;

    // re init both vects
    access_vect = std::vector<std::pair<time_t,thread_type>>(num_readers+num_writers);
    order_vect.clear();
    
    // reset shared data
    data = 0;

    // the threads
    pthread_t rthreads[num_readers];
    pthread_t wthreads[num_writers];

    // initializing mutex's
    sem_init(&mx_x,0,1);
    sem_init(&mx_y,0,1);
    sem_init(&mx_z,0,1);
    sem_init(&wsem,0,1);
    sem_init(&rsem,0,1);
    sem_init(&order_mux,0,1);

    // useful parameters for the loop
    long num_threads =0;
    bool toggle = true;
    long r_index=0,w_index=0; 
    long i; 

    // creates threads in alternating manner
    for (i=0;i<num_writers+num_readers;i++,num_threads++)
    {
        if (toggle)
        {
            if (r_index < num_readers)
            {
                pthread_create(&rthreads[r_index],NULL,reader,(void *)num_threads);
                r_index++; 
            } 
            else if(w_index < num_writers)
            {
                pthread_create(&wthreads[w_index],NULL,writer,(void *)num_threads);
                w_index++; 
            }
        }
        else
        {
            if(w_index < num_writers)
            {
                pthread_create(&wthreads[w_index],NULL,writer,(void *)num_threads);
                w_index++;
            }
            else if (r_index < num_readers)
            {
                pthread_create(&rthreads[r_index],NULL,reader,(void *)num_threads);
                r_index++;
            }

        }

        if (toggle)
            toggle = false;
        else
            toggle = true; 
    }

    // joins threads
    for (i=0; i < num_writers; i++)
    {
        pthread_join(wthreads[i],NULL);
    }
    for (i=0; i < num_readers; i++)
    {
        pthread_join(rthreads[i],NULL);
    }

    // sorts access vector
    std::sort(access_vect.begin(),access_vect.end());
}
