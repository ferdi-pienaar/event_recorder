/*
 * Composition root instantiates the objects and injects them into operator and event_handler.
 * This is the only file of the client that includes record_table.h; the other
 * files include only the abstract interfaces record_table_manager_itf.h, record_table_event_itf.h and
 * record_table_op_itf.h, so they don't depend on the implementation details in record_table.h.
 */
#include "event_generator.h"
#include "operator.h"
#include "operator_helper.h"
#include "record_table.h"
#include "record_table_manager.h"
#include "example_event_data_types.h"

constexpr unsigned NUM_ENTRIES = 3;

static Record_table<Double_stamp> ttable(Record_table_config().size(NUM_ENTRIES).enable(),
                                         dump_ts_array_cb, dump_table_state_cb);
static Record_table<int> itable(Record_table_config().size(NUM_ENTRIES).enable(), dump_int_cb,
                                dump_table_state_cb);
static Record_table_manager table_mgr({{"time", ttable}, {"int", itable}}, dump_name_cb);

int main(int argc, char * argv[])
{
    event_generator_init(ttable, itable);

    Operator oper;
    oper.init(table_mgr);
    return 0;
}
