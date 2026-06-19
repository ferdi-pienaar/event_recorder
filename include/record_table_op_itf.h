/*
 * Tie client uses this abstract interface to dump Record_table content and state,
 * using callback functions that the client registers when it creates the tables.
 */
#pragma once
#include "record_table_config.h"

// Operator's interface to Record_table -- commands to display entries and modify
// state of the table, but not to write entries, which is done via the event interface.
// xxx noexcept everywhere?
class Record_table_op_itf
{
public:
    virtual bool set_size(unsigned) noexcept = 0;
    virtual bool enable(bool) noexcept = 0;
    virtual bool oneshot(bool) noexcept = 0;
    virtual bool clear() noexcept = 0;
    // Call the registered dump callback for each written entry.
    virtual void dump() const = 0;
    virtual void dump_state() const = 0;
    virtual unsigned size() const noexcept = 0;
    virtual bool enabled() const noexcept = 0;
    virtual bool oneshot() const noexcept = 0;
    virtual bool active() const noexcept = 0;
    virtual bool is_stopped() const noexcept = 0;
    virtual unsigned get_num_written_entries() const noexcept = 0;
};
