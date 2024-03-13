/*
 * 'Factory' functions make a single object accessible to files that include this file.
 * To add a table, we do this:
 * Define it in a function in this file.
 * Register it with the manager.
 * Use it in 'recorder' code.
 * 
 * The alternative is to make it a global variable, but this approach seems better since
 * a function call is easier to search.
 * A disadvantage is that we essentially know a table by 2 names: the function name used here
 * and the string we use to register it with the manager. To make code easy to understand,
 * this names have to be manually kept similar.
 * 
 * An alternative would be to assign a Manager for each type of table, then the manager could
 * be a single point of access, giving 'recorder' clients access to the tables by name key.
 * In the current design, a Manager holds different types of tables, and there doesn't seem to
 * be a way it can return a reference to its tables by name on which the recorder client could
 * then call write_entry() and done().
 */
#pragma once

#include <string>
#include "record_table.h"
#include "record_table_iterator.h"
#include <array>
#include <time.h> // struct timespec
#include "record_table_helper.h"
#include "record_table_types.h"

inline Record_table<Double_stamp> & get_time_record_table()
{
    static Record_table<Double_stamp> t(Record_table_config().size(NUM_ENTRIES).enable(), dump_ts_array_cb, dump_table_state_cb);
    return t;
}

inline Record_table<int> & get_int_record_table()
{
    static Record_table<int> t(Record_table_config().size(NUM_ENTRIES).enable(), dump_int_cb, dump_table_state_cb);
    return t;
}
