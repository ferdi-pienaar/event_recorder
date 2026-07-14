/*
 * Composition root instantiates the objects and injects them into operator and event_handler.
 * This is the only file of the client that includes record_table.h; the other
 * files include only the abstract interfaces record_table_manager_itf.h, record_table_event_itf.h
 * and record_table_op_itf.h, so they don't depend on the implementation details in record_table.h.
 */
#include "event_generator.h"
#include "example_event_data_types.h"
#include "operator.h"
#include "operator_helper.h"
#include "record_table.h"
#include "record_table_manager.h"

using namespace Event_record;

int main(int argc, char *argv[])
{
    constexpr unsigned NUM_ENTRIES = 3;
    Table<Double_stamp> ttable(Table_init_config().size(NUM_ENTRIES).enable(), dump_ts_array_cb,
                               dump_table_state_cb);
    Int_dumper int_dumper;
    Table<int> itable(Table_init_config().size(NUM_ENTRIES).oneshot().enable(),
                      std::ref(int_dumper), dump_table_state_cb);
    Table_manager table_mgr({{"time", ttable}, {"int", itable}}, dump_name_cb);

    event_generator_init(ttable, itable);

    Operator oper(table_mgr);
    oper.run();
    return 0;
}
