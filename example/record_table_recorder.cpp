/*
 *
 */

#include "record_table_defs.h"
#include "record_table_recorder.h"

static auto & ttable = get_time_record_table();
static auto & itable = get_int_record_table();

// Save an entry, begin and end for an event.
void time_event()
{
    auto & w_entry = ttable.write_entry();
    for (unsigned j = 0; j < NUM_STAMPS_PER_ENTRY; ++j)
    {
        clock_gettime(CLOCK_REALTIME, &w_entry[j]);
    }
}

void int_event()
{
    static int counter = 0;
    itable.write_entry() = counter++;
}
