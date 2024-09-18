/*
 *
 */

#include "record_table_helper.h"
#include <iostream>
#include <string>

void dump_ts_array_cb(const std::array<timespec, NUM_STAMPS_PER_ENTRY> & entry)
{
    std::cout << "entry size " << entry.size() << std::endl;

    for (auto const & t : entry)
    {
        std::cout << t.tv_sec << "." << t.tv_nsec << std::endl;
    }
}

void dump_int_cb(const int & entry)
{
    std::cout << entry << std::endl;
}

void dump_table_state_cb(const Record_table_generic & t)
{
    std::cout << "enabled " << t.get_config().m_enabled << std::endl;
    std::cout << "oneshot " << t.get_config().m_oneshot << std::endl;
    std::cout << "size " << t.get_config().m_size << std::endl;
    std::cout << "written entries " << t.get_num_written_entries() << std::endl;
    std::cout << "stopped " << t.is_stopped() << std::endl;
}

void dump_name_cb(const std::string & name)
{
    std::cout << "table '"<< name << "'" << std::endl;
}
