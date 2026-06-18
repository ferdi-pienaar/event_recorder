/*
 * It allows a client function (register at init) to dump a record table's state
 * without having to specify the ENTRY parameter of the Record_table template,
 * since the state is independent of the ENTRY type.
 *
 */

#pragma once
#include "record_table_config.h"

// Operator's interface to Record_table -- commands to display entries and modify
// state of the table, but not to write entries, which is via the event interface.
// xxx noexcept everywhere?
// xxx dump_state calls the dump_state function registered by the client, which in turn calls
// is_stopped, get_num_written_ and get_config. Does this back-and-forth make sense?
class Record_table_op_itf
{
public:
    virtual bool enable(bool) noexcept = 0;
    virtual bool set_size(unsigned) noexcept = 0;
    virtual bool clear() noexcept = 0;
    // Call the registered dump callback for each written entry.
    virtual void dump() const = 0;
    //virtual bool enabled() const noexcept = 0;
    //virtual bool active() const noexcept = 0;
    virtual bool oneshot(bool) noexcept = 0;
    virtual void dump_state() const = 0;
    virtual bool is_stopped() const = 0;
    virtual unsigned get_num_written_entries() const = 0;
    virtual const Record_table_config & get_config() const = 0;
};
