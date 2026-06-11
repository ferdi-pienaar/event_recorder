/*
 * A record table holds data of type given by its template parameter ENTRY.
 * It has two interfaces (not currently implemented as separate interfaces
 * in code):
 * The 'record' interface allows instrumented code to:
 *  - get_write_entry returns a writable ENTRY in RAM. This could be e.g.
 *    a timestamp generated at run-time.
 *  - Indicate if the entry is complete and the next call to get_write_entry returns
 *    a new entry. True by default. If instrumented code chooses 'false', the
 *    same entry is returned by the next call to get_write_entry, e.g. to overwrite
 *    or modify an entry.
 *
 * The 'operator' interface allows an operator to work with the records of ENTRYs.
 * - Enable: when enabled, instrumented code saves ENTRYs, else none are saved.
 * - set_size: change max number of ENTRYs that instrumented code can save.
 * - Clear: deleted saved ENTRYs.
 * - Dump: call a registered dump function to examine saved ENTRYs.
 * - Dump state: call a callback function to dump state such as enabled/disabled,
 *   onshot/overwrite mode, size (max), number of saved entries, size, is-stopped
 *   (e.g. if oneshot and full).
 * - Change oneshot/overwrite mode: oneshot means write until table is full
 *   (don't overwrite old entries); overwrite means repeatedly overwrite old entries
 *   with new ones.
 *
 */

#pragma once
#include "record_table_generic.h"
#include <string>
#include <vector>
#include <map>
#include <assert.h>

template <typename ENTRY>
class Record_table_iterator;

template <typename ENTRY>
class Record_table : public Record_table_generic
{
public:
    using DUMP_CALLBACK = void (*)(const ENTRY &);

    Record_table(const Record_table_config & config = Record_table_config::CONFIG_DEFAULT,
                 DUMP_CALLBACK cb = nullptr,
                 Record_table_generic::DUMP_STATE_CALLBACK dump_state_cb = nullptr);
    ~Record_table();
    // 'Record' interface consists of get_write_entry() and optional done().
    // Returns a reference to an entry to write to.
    // Client uses default complete=true if it wants to move on the next entry, or false
    // if it wants to access the current entry again.
    auto & get_write_entry(bool complete = true) noexcept;
    // Client may call done after get_write_entry, to move on to next entry.
    // Calling get_write_entry(false) followed by done() is equivalent to calling get_write_entry
    // without params.
    void done() noexcept;

    // Operator interface.
    bool enable(bool) noexcept override;
    bool set_size(unsigned) noexcept override;
    bool clear() noexcept override;
    // Call the registered dump callback for each written entry.
    void dump() const override;

    friend class Record_table_iterator<ENTRY>;

private:
    void allocate_entries();
    void free_entries();
    void advance() noexcept;
    ENTRY * next(ENTRY * entry) const noexcept;

    ENTRY * m_entries = nullptr;
    ENTRY * m_write = nullptr; // Entry to write to.
    ENTRY * m_end = nullptr; // Pointer past the end of the allocated entries.
    ENTRY m_dummy_entry; // Entry returned to client if disabled: client may write to it without effect.
    const DUMP_CALLBACK m_dump_cb = nullptr;
};

template <typename ENTRY>
Record_table<ENTRY>::Record_table(const Record_table_config & config,
                                  DUMP_CALLBACK cb,
                                  Record_table_generic::DUMP_STATE_CALLBACK dump_state_cb) :
    Record_table_generic(config, dump_state_cb),
    m_dump_cb(cb)
{
    if (m_config.m_enabled)
    {
        allocate_entries();
    }
}

template <typename ENTRY>
Record_table<ENTRY>::~Record_table()
{
    free_entries();
}

template <typename ENTRY>
auto & Record_table<ENTRY>::get_write_entry(bool complete) noexcept
{
    if (!active())
    {
        return m_dummy_entry;
    }
    ENTRY & entry = *m_write;
    if (complete)
    {
        advance();
    }
    return entry;
}

template <typename ENTRY>
void Record_table<ENTRY>::done() noexcept
{
    if (active())
    {
        advance();
    }
}

template <typename ENTRY>
ENTRY * Record_table<ENTRY>::next(ENTRY * entry) const noexcept
{
    auto nxt = ++entry;
    if (nxt == m_end)
    {
        // Rollover
        return m_entries;
    }
    return nxt;
}

template <typename ENTRY>
void Record_table<ENTRY>::advance() noexcept
{
    m_write = next(m_write);
    if (m_num_written_entries < m_config.m_size)
    {
        ++m_num_written_entries;
    }
    if (m_config.m_oneshot && (m_num_written_entries == m_config.m_size))
    {
        m_stopped = true;
    }
}

// @pre no memory allocated currently.
template <typename ENTRY>
void Record_table<ENTRY>::allocate_entries()
{
    assert(m_entries == nullptr);

    m_entries = new ENTRY[m_config.m_size];
    m_end = m_entries + m_config.m_size;

    m_write = m_entries;
}

template <typename ENTRY>
void Record_table<ENTRY>::free_entries()
{
    if (m_entries != nullptr)
    {
        delete[] m_entries;
        m_entries = nullptr;
        clear();
    }
}

// This clears what has been written, but does not free entry memory.
template <typename ENTRY>
bool Record_table<ENTRY>::clear() noexcept
{
    if (m_config.m_enabled)
    {
        return false;
    }

    m_num_written_entries = 0;
    m_write = m_entries;
    m_stopped = false;
    return true;
}

template <typename ENTRY>
bool Record_table<ENTRY>::enable(bool ena) noexcept
{
    if (ena)
    {
        if (m_config.m_size == 0)
        {
            // Can't enable if size is 0.
            return false;
        }

        if (m_entries == nullptr)
        {
            // Allocate if not done, e.g. first enable, or first enable after size change.
            allocate_entries();
        }
    }
    m_config.m_enabled = ena;
    return true;
}

template <typename ENTRY>
bool Record_table<ENTRY>::set_size(unsigned size) noexcept
{
    if (enabled())
    {
        // xxx WARNING.
        return false;
    }

    m_config.m_size = size;
    // Free allocated memory -- we assume the new size value is different from
    // the current value.
    // Memory will be allocated if client enables.
    free_entries();
    return true;
}

// xxx only if not enabled?
template <typename ENTRY>
void Record_table<ENTRY>::dump() const
{
    if (m_dump_cb == nullptr)
    {
        return;
    }
    Record_table_iterator<ENTRY> iter(*this);
    for (iter.begin(); !iter.end(); iter.next())
    {
        m_dump_cb(iter.get_current());
    }
}
