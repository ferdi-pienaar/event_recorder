/*
 * Helper functions used by the operator that manages and dumps the record tables.
 */
#include <string>

class Record_table_op_itf;
struct timespec;

void dump_ts_cb(const timespec & entry);

void dump_table_state_cb(const Record_table_op_itf & t);
void dump_name_cb(const std::string &);
