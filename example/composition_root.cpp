/*
 * Composition root instantiates the classes and injects them into
 * operator and event_handler.
 */
#include "event_generator.h"
#include "operator.h"
#include "operator_helper.h"
#include "record_table.h"
#include "record_table_manager.h"
#include "example_event_data_types.h"

constexpr unsigned NUM_ENTRIES = 3;

static Record_table<Double_stamp> ttable(Record_table_config().size(NUM_ENTRIES).enable(), dump_ts_array_cb, dump_table_state_cb);
static Record_table<int> itable(Record_table_config().size(NUM_ENTRIES).enable(), dump_int_cb, dump_table_state_cb);
static Record_table_manager table_mgr({{"time", ttable}, {"int", itable}}, dump_name_cb);

// xxx remove
//static std::map<std::string, Record_table_manager_interface &> managers = {{"time", get_timestamp_manager()}, {"int", get_int_manager()}};

int main(int argc, char * argv[])
{
    event_generator_init(ttable, itable);
    operator_init(table_mgr);
    return 0;
}
