/*
 * Helper functions used by the operator that manages and dumps the record tables.
 * It depends only on the abstract interface record_table_op_itf.h.
 */
#include "operator_helper.h"
#include "record_table_op_itf.h"
#include <iostream>
#include <string>

using namespace Event_record;

void dump_ts_array_cb(const std::array<timespec, NUM_STAMPS_PER_ENTRY> &entry)
{
    std::cout << "entry size " << entry.size() << std::endl;

    for (auto const &t : entry)
    {
        std::cout << t.tv_sec << "." << t.tv_nsec << std::endl;
    }
}

// A functor that can be registered as callback.
void Int_dumper::operator()(const int &entry)
{
    std::cout << entry << std::endl;
}

void dump_table_state_cb(const Table_op_itf &t)
{
    std::cout << "size " << t.size() << std::endl;
    std::cout << "enabled " << t.enabled() << std::endl;
    std::cout << "oneshot " << t.oneshot() << std::endl;
    std::cout << "written entries " << t.get_num_written_entries() << std::endl;
    std::cout << "stopped " << t.is_stopped() << std::endl;
}

void dump_name_cb(const std::string &name)
{
    std::cout << "table '" << name << "'" << std::endl;
}
