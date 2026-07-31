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
    TableRollover<Double_stamp> ttable(Table_init_config().size(NUM_ENTRIES).enable(),
                                       dump_ts_array_cb, dump_table_state_cb);
    Int_dumper int_dumper;
    TableOneshot<int> itable(Table_init_config().size(NUM_ENTRIES).enable(), std::ref(int_dumper),
                             dump_table_state_cb);
    Table<int> itable2(Table_init_config().size(NUM_ENTRIES).enable().oneshot(), std::ref(int_dumper),
                             dump_table_state_cb);
    Table_manager table_mgr({{"time", ttable}, {"int-oneshot", itable}, {"int-select", itable2}}, operator_out_cb);

    event_generator_init(ttable, itable, itable2);

    Operator oper(table_mgr);
    oper.run();
    return 0;
}
