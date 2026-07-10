/* The purpose of placing this in a separate file is so compiler optimization can't remove the
 * virtual function calls implicit in using the abstract interface Record_table_event_itf<int>.
 * However, that does not seem to be working -- code disassembly doesn't show use of virtual
 * function vtable.
 */
#include "get_table_itf.h"
#include "record_table.h"

Record_table_event_itf<int> * get_table_itf()
{
    static Record_table<int> table(Record_table_config().size(10).enable());
    return &table;
}
