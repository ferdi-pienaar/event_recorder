/*
 * The client uses this abstract operator interface to dump Event_record::Table contents and state
 * using callback functions that the client registers when it creates the tables. The client can
 * also modify Table config. The following operations clear the table: clear, set_size.
 */
#pragma once
#include "record_table_init_config.h"

namespace Event_record
{

// Operator's interface to Table.
// xxx noexcept everywhere?

class Table_op_itf
{
public:
    virtual bool set_size(unsigned) noexcept = 0;
    virtual bool enable(bool) noexcept = 0;
    virtual bool oneshot(bool) noexcept = 0;
    virtual bool clear() noexcept = 0;
    // Call the registered dump callback for each written entry.
    virtual bool dump() const = 0;
    virtual bool dump_state() const = 0;
    virtual unsigned size() const noexcept = 0;
    virtual bool enabled() const noexcept = 0;
    virtual bool oneshot() const noexcept = 0;
    virtual bool active() const noexcept = 0;
    virtual bool is_stopped() const noexcept = 0;
    virtual unsigned get_num_written_entries() const noexcept = 0;
};

} // namespace Event_record
