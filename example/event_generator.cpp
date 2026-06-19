/*
 * This depends only on the abstract interface record_table_event_itf.h.
 */
#include "event_generator.h"
#include "record_table_event_itf.h"
#include <pthread.h>
#include <iostream>
#include <unistd.h> // sleep

// Pointers initialized by injection at initialization.
static Record_table_event_itf<Double_stamp> *ttable = nullptr;
static Record_table_event_itf<int> *itable = nullptr;

static void *worker(void *arg);
static void time_event();
static void int_event(unsigned int);

int event_generator_init(Record_table_event_itf<Double_stamp> &tt, Record_table_event_itf<int> &it)
{
    ttable = &tt;
    itable = &it;

    // Spawn a thread in which we generate events periodically.
    pthread_t thread;

    // Create thread
    int result = pthread_create(&thread, nullptr, worker, nullptr);
    if (result != 0)
    {
        std::cerr << "Error: pthread_create failed (" << result << ")\n";
        return 1;
    }

    // Don't wait for the thread to join because: it won't happen.
    std::cout << "Thread created.\n";
    return 0;
}

void *worker(void *arg)
{
    while (true)
    {
        static unsigned int counter = 0;
        sleep(2);
        time_event();
        int_event(counter);
        counter++;
    }
    return nullptr;
}

// Save an entry, begin and end for an event.
void time_event()
{
    auto & w_entry = ttable->get_write_entry();
    for (unsigned j = 0; j < NUM_STAMPS_PER_ENTRY; ++j)
    {
        clock_gettime(CLOCK_REALTIME, &w_entry[j]);
    }
}

void int_event(unsigned int c)
{
    if (c % 3 == 0)
    {
        itable->get_write_entry() = c * 10;
    }
}
