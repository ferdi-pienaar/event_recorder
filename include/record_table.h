/*
 * A record table holds data of type given by its template parameter ENTRY.
 * It has two interfaces (not currently implemented as separate interfaces
 * in code):
 * The 'record' interface allows instrumented code to:
 * - get_write_entry returns a writable ENTRY in RAM. This could be e.g.
 *   a timestamp generated at run-time.
 * - Indicate if the entry is complete and the next call to get_write_entry returns a new entry.
 *   True by default. If instrumented code chooses 'false', the same entry is returned by the next
 *   call to get_write_entry, e.g. to overwrite or modify an entry.
 *
 * The 'operator' interface allows an operator to work with the records of ENTRYs.
 * - Enable: when enabled, instrumented code saves ENTRYs, else none are saved.
 * - set_size: change max number of ENTRYs that instrumented code can save.
 * - Clear: deleted saved ENTRYs.
 * - Dump: call a registered dump function to examine saved ENTRYs.
 * - Dump state: call a callback function to dump state such as enabled/disabled,
 *   onshot/overwrite mode, size (max), number of saved entries, size, is-stopped
 *   (e.g. if oneshot and full).
 * - Change oneshot/overwrite mode: oneshot means write until table is full (don't overwrite old
 *   entries); overwrite means repeatedly overwrite old entries with new ones.
 */
#pragma once
#include "record_table_event_itf.h"
#include "record_table_op_itf.h"
#include "record_table_config.h"
#include "record_table_iterator.h" // used in dump method.
#include <string>
#include <functional>
#include <assert.h>

template <typename ENTRY>
class Record_table_iterator;

template <typename ENTRY>
class Record_table : public Record_table_event_itf<ENTRY>, public Record_table_op_itf
{
public:
    using DUMP_CALLBACK = std::function<void(const ENTRY &)>;
    using DUMP_STATE_CALLBACK = std::function<void(const Record_table_op_itf &)>;

    Record_table(const Record_table_config & config = Record_table_config::CONFIG_DEFAULT,
                 DUMP_CALLBACK cb = nullptr,
                 DUMP_STATE_CALLBACK dump_state_cb = nullptr);
    ~Record_table();
    // 'Record' interface consists of get_write_entry() and optional done().
    // Returns a reference to an entry to write to.
    ENTRY & get_write_entry() noexcept override;
    void stop() noexcept override { m_stopped = true; };

    // Operator interface.
    bool set_size(unsigned) noexcept override;
    bool enable(bool) noexcept override;
    bool oneshot(bool) noexcept override;
    bool clear() noexcept override;
    // Call the registered dump callback for each written entry.
    bool dump() const override;
    bool dump_state() const override;
    unsigned size() const noexcept override { return m_config.get_size(); }
    bool enabled() const noexcept override { return m_config.get_enabled(); }
    bool oneshot() const noexcept override { return m_config.get_oneshot(); }
    bool active() const noexcept override { return m_config.get_enabled() && !m_stopped; }
    bool is_stopped() const noexcept override { return m_stopped; }
    unsigned get_num_written_entries() const noexcept override { return m_num_written_entries; }

    friend class Record_table_iterator<ENTRY>;

private:
    void allocate_entries();
    void free_entries();
    void advance() noexcept;
    ENTRY * next(ENTRY * entry) const noexcept;

    const DUMP_CALLBACK m_dump_cb = nullptr;
    Record_table_config m_config;
    const DUMP_STATE_CALLBACK m_dump_state_cb = nullptr;
    ENTRY * m_entries = nullptr;
    ENTRY * m_write = nullptr; // Entry to write to.
    ENTRY * m_end = nullptr; // Pointer past the end of the allocated entries.
    ENTRY m_dummy_entry; // Entry returned to client if disabled: client may write to it without effect.
    unsigned m_num_written_entries = 0; // Number of written entries, capped at config.size.
    bool m_stopped = false; // One-shot full => true, clear => false.
};

template <typename ENTRY>
Record_table<ENTRY>::Record_table(const Record_table_config & config,
                                  DUMP_CALLBACK cb,
                                  DUMP_STATE_CALLBACK dump_state_cb) :
    m_config(config), m_dump_state_cb(dump_state_cb), m_dump_cb(cb)
{
    if (m_config.get_enabled())
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
ENTRY & Record_table<ENTRY>::get_write_entry() noexcept
{
    if (!active())
    {
        return m_dummy_entry;
    }
    ENTRY & entry = *m_write;
    advance();
    return entry;
}

template <typename ENTRY>
bool Record_table<ENTRY>::set_size(unsigned size) noexcept
{
    if (m_config.get_enabled())
    {
        // xxx WARNING -- don't allow modifying size while enabled.
        return false;
    }

    m_config.set_size(size);
    // Free allocated memory -- even if the new size is the same as the current size.
    // Memory will be allocated if client enables.
    free_entries();
    return true;
}

template <typename ENTRY>
bool Record_table<ENTRY>::enable(bool ena) noexcept
{
    if (ena)
    {
        if (m_config.get_size() == 0)
        {
            // Can't enable if size is 0.
            return false;
        }

        if (m_entries == nullptr)
        {
            // Allocate if not done, e.g. first enable, or first enable after changing the size.
            allocate_entries();
        }
    }

    m_config.set_enabled(ena);
    return true;
}

// Note that if operator switches to oneshot mode when the table is already full, we'll
// set m_stopped after one more entry is added in oneshot mode.
template <typename ENTRY>
bool Record_table<ENTRY>::oneshot(bool mode) noexcept
{
    if (m_config.get_enabled())
    {
        return false;
    }

    if (mode == false)
    {
        // In rollover mode, we don't stop.
        m_stopped = false;
    }
    m_config.set_oneshot(mode);
    return true;
}

// This clears what has been written, but does not free entry memory.
template <typename ENTRY>
bool Record_table<ENTRY>::clear() noexcept
{
    if (m_config.get_enabled())
    {
        return false;
    }

    m_num_written_entries = 0;
    m_write = m_entries;
    m_stopped = false;
    return true;
}

// xxx only if not enabled?
template <typename ENTRY>
bool Record_table<ENTRY>::dump() const
{
    if (m_dump_cb == nullptr)
    {
        return false;
    }
    Record_table_iterator<ENTRY> iter(*this);
    for (iter.begin(); !iter.end(); iter.next())
    {
        m_dump_cb(iter.get_current());
    }
    return true;
}

template <typename ENTRY>
bool Record_table<ENTRY>::dump_state() const
{
    if (m_dump_state_cb == nullptr)
    {
        return false;
    }
    m_dump_state_cb(*this);
    return true;
}

// @pre no memory allocated currently.
template <typename ENTRY>
void Record_table<ENTRY>::allocate_entries()
{
    assert(m_entries == nullptr);

    m_entries = new ENTRY[m_config.get_size()];
    m_end = m_entries + m_config.get_size();

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

template <typename ENTRY>
void Record_table<ENTRY>::advance() noexcept
{
    m_write = next(m_write);
    if (m_num_written_entries < m_config.get_size())
    {
        ++m_num_written_entries;
    }
    if (m_config.get_oneshot() && (m_num_written_entries == m_config.get_size()))
    {
        m_stopped = true;
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
