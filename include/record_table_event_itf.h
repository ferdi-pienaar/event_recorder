/*
 * It allows a client xxx
 *
 */

#pragma once

template <typename ENTRY>
class Record_table_event_itf
{
public:
    virtual ENTRY & get_write_entry(bool complete = true) noexcept = 0;
    // Client may call done after get_write_entry, to move on to next entry.
    // Calling get_write_entry(false) followed by done() is equivalent to calling get_write_entry
    // without params.
    virtual void done() noexcept = 0;
};
