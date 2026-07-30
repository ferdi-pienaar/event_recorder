/*
 * A Table holds data of type given by its template parameter ENTRY.
 * It has two interfaces:
 * The 'event' interface allows instrumented code to:
 * - get_write_entry returns a writable ENTRY in RAM. This could be e.g.
 *   a timestamp generated at run-time.
 * - stop: disable writing new entries, e.g. when something has happened such
 *   as an error, and we don't want to save new entries since they could overwrite
 *   old entries which may give a clue to the source of the error.
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
#include "rollover_type.h"
#include "record_table_config.h"
#include "record_table_event_itf.h"
#include "record_table_init_config.h"
#include "record_table_iterator.h" // used in dump method.
#include "record_table_op_itf.h"
#include <assert.h>
#include <functional>
#include <string>

namespace Event_record
{

template <typename ENTRY, RolloverType RT> class Table_iterator;

template <typename ENTRY, RolloverType RT = RolloverType::RuntimeSelect>
class Table : public Table_event_itf<ENTRY>, public Table_op_itf
{
public:
    using DUMP_CALLBACK = std::function<void(const ENTRY &)>;
    using DUMP_STATE_CALLBACK = std::function<void(const Table_op_itf &)>;

    Table(const Table_init_config &config = Table_init_config::CONFIG_DEFAULT,
          DUMP_CALLBACK cb = nullptr, DUMP_STATE_CALLBACK dump_state_cb = nullptr);
    ~Table();
    // 'Record' interface consists of get_write_entry() and optional done().
    // Returns a reference to an entry to write to.
    ENTRY &get_write_entry() noexcept override final;
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
    bool oneshot() const noexcept override;
    bool active() const noexcept override { return m_config.get_enabled() && !m_stopped; }
    bool is_stopped() const noexcept override { return m_stopped; }
    unsigned get_num_written_entries() const noexcept override { return m_num_written_entries; }

    friend class Table_iterator<ENTRY, RT>;

private:
    void allocate_entries();
    void free_entries();
    void advance() noexcept;
    ENTRY *next(ENTRY *entry) const noexcept;

    const DUMP_CALLBACK m_dump_cb = nullptr;
    Table_config m_config;
    const DUMP_STATE_CALLBACK m_dump_state_cb = nullptr;
    ENTRY *m_entries = nullptr;
    ENTRY *m_write = nullptr; // Entry to write to.
    ENTRY *m_end = nullptr;   // Pointer past the end of the allocated entries.
    ENTRY m_dummy_entry; // Entry returned to client if disabled: client may write to it without
                         // effect.
    unsigned m_num_written_entries = 0; // Number of written entries, capped at config.size.
    bool m_stopped = false;             // One-shot full => true, clear => false.
};

template <typename ENTRY, RolloverType RT>
Table<ENTRY, RT>::Table(const Table_init_config &config, DUMP_CALLBACK cb,
                    DUMP_STATE_CALLBACK dump_state_cb)
    : m_config(config), m_dump_state_cb(dump_state_cb), m_dump_cb(cb)
{
    if (m_config.get_enabled())
    {
        allocate_entries();
    }
}

template <typename ENTRY, RolloverType RT> Table<ENTRY, RT>::~Table()
{
    free_entries();
}

template <typename ENTRY, RolloverType RT> ENTRY &Table<ENTRY, RT>::get_write_entry() noexcept
{
    if (!active())
    {
        return m_dummy_entry;
    }
    ENTRY &entry = *m_write;
    advance();
    return entry;
}

template <typename ENTRY, RolloverType RT> bool Table<ENTRY, RT>::set_size(unsigned size) noexcept
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

template <typename ENTRY, RolloverType RT> bool Table<ENTRY, RT>::enable(bool ena) noexcept
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
template <typename ENTRY, RolloverType RT>
bool Table<ENTRY, RT>::oneshot(bool mode_oneshot) noexcept
{
    // Accept setting mode to oneshot, reject rollover.
    if (RT == RolloverType::Oneshot) return mode_oneshot;

    // Accept setting mode to rollover, reject oneshot.
    if (RT == RolloverType::Rollover) return not mode_oneshot;

    // Mode can be modified at run-time, so modify it (if not enabled).
    if (m_config.get_enabled())
    {
        return false;
    }

    if (!mode_oneshot)
    {
        // In rollover mode, we don't stop.
        m_stopped = false;
    }
    m_config.set_oneshot(mode_oneshot);
    return true;
}

// This clears what has been written, but does not free entry memory.
template <typename ENTRY, RolloverType RT> bool Table<ENTRY, RT>::clear() noexcept
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
template <typename ENTRY, RolloverType RT> bool Table<ENTRY, RT>::dump() const
{
    if (m_dump_cb == nullptr)
    {
        return false;
    }
    Table_iterator<ENTRY, RT> iter(*this);
    for (iter.begin(); !iter.end(); iter.next())
    {
        m_dump_cb(iter.get_current());
    }
    return true;
}

template <typename ENTRY, RolloverType RT> bool Table<ENTRY, RT>::dump_state() const
{
    if (m_dump_state_cb == nullptr)
    {
        return false;
    }
    m_dump_state_cb(*this);
    return true;
}

template <typename ENTRY, RolloverType RT> bool Table<ENTRY, RT>::oneshot() const noexcept
{
    if (RT == RolloverType::Oneshot) return true;
    if (RT == RolloverType::Rollover) return false;
    return m_config.get_oneshot();
}

// @pre no memory allocated currently.
template <typename ENTRY, RolloverType RT> void Table<ENTRY, RT>::allocate_entries()
{
    assert(m_entries == nullptr);

    m_entries = new ENTRY[m_config.get_size()];
    m_end = m_entries + m_config.get_size();

    m_write = m_entries;
}

template <typename ENTRY, RolloverType RT> void Table<ENTRY, RT>::free_entries()
{
    if (m_entries != nullptr)
    {
        delete[] m_entries;
        m_entries = nullptr;
        clear();
    }
}

template <typename ENTRY, RolloverType RT> void Table<ENTRY, RT>::advance() noexcept
{
    m_write = next(m_write);
    if (m_num_written_entries < m_config.get_size())
    {
        ++m_num_written_entries;
    }

    if (RT == RolloverType::Rollover) return;

    if (m_num_written_entries == m_config.get_size())
    {
        // Full, so set stopped if oneshot
        if ((RT == RolloverType::Oneshot) || m_config.get_oneshot())
        {
            m_stopped = true;
        }
    }
}

template <typename ENTRY, RolloverType RT>
ENTRY *Table<ENTRY, RT>::next(ENTRY *entry) const noexcept
{
    auto nxt = ++entry;
    if (nxt == m_end)
    {
        // Rollover
        return m_entries;
    }
    return nxt;
}

template<typename ENTRY>
using TableRollover = Table<ENTRY, RolloverType::Rollover>;

template<typename ENTRY>
using TableOneshot = Table<ENTRY, RolloverType::Oneshot>;

} // namespace Event_record
