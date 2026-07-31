/*
 * Helper functions used by the operator that manages and dumps the record tables.
 */
#include "example_event_data_types.h"
#include <string>

namespace Event_record
{
class Table_op_itf;
}

void dump_ts_array_cb(const Double_stamp &entry);

class Int_dumper
{
public:
    void operator()(const int &entry);
};

void dump_table_state_cb(const Event_record::Table_op_itf &t);
void operator_out_cb(const std::string &);
