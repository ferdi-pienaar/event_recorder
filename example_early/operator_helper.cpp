/*
 * Helper functions used by the operator that manages and dumps the record tables.
 * It depends only on the abstract interface record_table_op_itf.h.
 */
#include "operator_helper.h"
#include "record_table_op_itf.h"
#include <iostream>
#include <string>
#include <time.h> // struct timespec

using namespace Event_record;

void dump_ts_cb(const timespec &entry)
{
    std::cout << entry.tv_sec << "." << entry.tv_nsec << std::endl;
}

void dump_table_state_cb(const Table_op_itf &t)
{
    std::cout << "size " << t.size() << std::endl;
    std::cout << "enabled " << t.enabled() << std::endl;
    std::cout << "oneshot " << t.oneshot() << std::endl;
    std::cout << "written entries " << t.get_num_written_entries() << std::endl;
    std::cout << "stopped " << t.is_stopped() << std::endl;
}

void operator_out_cb(const std::string &msg)
{
    std::cout << msg;
}
