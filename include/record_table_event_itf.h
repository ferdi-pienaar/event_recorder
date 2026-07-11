/*
 * The 'write' interface of Record_table. The client uses it to write data about an event in a Record_table.
 * xxx SHould it include an is_active member fn?
 */
#pragma once

template <typename ENTRY>
class Record_table_event_itf
{
public:
    virtual ENTRY & get_write_entry() noexcept = 0;
    // Call stop when an event of interest has happened, and we don't want to roll over
    // captured entries that record what happened before the event.
    virtual void stop() noexcept = 0;
};
