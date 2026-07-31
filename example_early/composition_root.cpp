/*
 * Composition root instantiates the objects and injects them into operator and event_handler.
 * This is the only file of the client that includes record_table.h; the other
 * files include only the abstract interfaces record_table_manager_itf.h, record_table_event_itf.h
 * and record_table_op_itf.h, so they don't depend on the implementation details in record_table.h.
 */
#include "event_generator.h"
#include "operator.h"
#include "operator_helper.h"
#include "record_table.h"
#include "record_table_manager.h"

using namespace Event_record;

// Returns a reference to an initialized table. This may be called before main() runs;
// table is initialized the first time this function is called.
static Table<timespec> table(Table_init_config().size(4).enable(), dump_ts_cb, dump_table_state_cb);

// Create Event_generator and record an event before main() runs.
Event_generator ev_gen(table);
int res = ev_gen.run();

int main(int argc, char *argv[])
{
    ev_gen.run();

    Table_manager table_mgr({{"time", table}}, operator_out_cb);
    Operator oper(table_mgr);

    ev_gen.run();
    oper.run();
    return 0;
}
