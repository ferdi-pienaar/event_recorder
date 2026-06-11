/*
 *
 */

//#include "record_table_defs.h"
#include "record_table_types.h"
#include "record_table_recorder.h"
#include "record_table_manager.h"
#include "record_table_operator.h"

static auto ttable = get_timestamp_manager().get_table("time1");
static auto itable = get_int_manager().get_table("int1");

// Save an entry, begin and end for an event.
void time_event()
{
    auto & w_entry = ttable->get_write_entry();
    for (unsigned j = 0; j < NUM_STAMPS_PER_ENTRY; ++j)
    {
        clock_gettime(CLOCK_REALTIME, &w_entry[j]);
    }
}

void int_event()
{
    static int counter = 0;
    itable->get_write_entry() = counter++;
}
