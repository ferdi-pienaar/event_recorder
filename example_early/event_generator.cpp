/*
 * This depends only on the abstract interface record_table_event_itf.h.
 */
#include "event_generator.h"
#include "record_table_event_itf.h"
#include <time.h> // struct timespec

using namespace Event_record;

Event_generator::Event_generator(Table_event_itf<timespec> &t) : m_table(t) {}

int Event_generator::run()
{
    auto &w_entry = m_table.get_write_entry();
    clock_gettime(CLOCK_REALTIME, &w_entry);
    return 0;
}