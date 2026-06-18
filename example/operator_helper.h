/*
 * Helper functions used by the operator that manages and dumps the record tables.
 */
#include "example_event_data_types.h"
#include <string>

class Record_table_op_itf;

void dump_ts_array_cb(const std::array<timespec, NUM_STAMPS_PER_ENTRY> & entry);
void dump_int_cb(const int & entry);
void dump_table_state_cb(const Record_table_op_itf & t);
void dump_name_cb(const std::string &);
