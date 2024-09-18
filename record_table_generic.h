/*
 *
 *
 */

#pragma once
#include "record_table_config.h"

// Code that's not specific to the type of entry in derived Record_table class template.
class Record_table_generic
{
public:
    using DUMP_STATE_CALLBACK = void (*)(const Record_table_generic &);

    Record_table_generic(const Record_table_config &, DUMP_STATE_CALLBACK dump_state);

    // Operator interface.
    bool enabled() const noexcept;
    bool oneshot(bool) noexcept;
    bool active() const noexcept;
    virtual bool enable(bool) = 0;
    virtual bool size(unsigned) = 0;
    virtual bool clear() = 0;
    virtual void dump() const = 0;
    void dump_state() const;
    const Record_table_config & get_config() const noexcept { return m_config; }
    bool is_stopped() const noexcept { return m_stopped; }
    unsigned get_num_written_entries() const noexcept { return m_num_written_entries; }

protected:
    Record_table_config m_config;
    const DUMP_STATE_CALLBACK m_dump_state = nullptr;
    unsigned m_num_written_entries = 0; // Number of written entries, capped at config.size.
    bool m_stopped = false; // One-shot full => true, clear => false. xxx could also be in Record_table.
};
