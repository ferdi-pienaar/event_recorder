/*
 *
 */
#include "record_table_types.h"
#include "record_table_generic.h"
#include <string>

void dump_ts_array_cb(const std::array<timespec, NUM_STAMPS_PER_ENTRY> & entry);
void dump_int_cb(const int & entry);
void dump_table_state_cb(const Record_table_generic & t);
void dump_name_cb(const std::string &);
