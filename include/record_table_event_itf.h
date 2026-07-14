/*
 * The 'write' interface of Event_record::Table. The client uses it to write data about an event in
 * a Event_record::Table. xxx SHould it include an is_active member fn?
 */
#pragma once

namespace Event_record
{

template <typename ENTRY> class Table_event_itf
{
public:
    virtual ENTRY &get_write_entry() noexcept = 0;
    // Call stop when an event of interest has happened, and we don't want to roll over
    // captured entries that record what happened before the event.
    virtual void stop() noexcept = 0;
};

} // namespace Event_record
