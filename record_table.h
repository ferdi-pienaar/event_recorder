/*
 *
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
    Record_table(const Record_table_config & config = Record_table_config::CONFIG_DEFAULT);
    ~Record_table();
    // Client calls advance before write_entry, unless it wants to access the
    // current entry again, to overwrite it or add something.
    Record_table & advance();
    ENTRY & write_entry();
    ENTRY * next(ENTRY * entry) const;
    bool enable(bool);
    bool size(unsigned);
    void clear();

    friend class Record_table_iterator<ENTRY>;

private:
    void allocate_entries();
    void free_entries();

    ENTRY * m_entries = nullptr;
    ENTRY * m_write = nullptr; // Last entry written to.
    ENTRY * m_end = nullptr; // Pointer past the end of the allocated entries.
    ENTRY m_dummy_entry; // Entry returned to client if disabled.
};

template <typename ENTRY>
Record_table<ENTRY>::Record_table(const Record_table_config & config) : Record_table_generic(config)
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
Record_table<ENTRY> & Record_table<ENTRY>::advance()
{
    if (m_config.m_enabled)
    {
        m_write = next(m_write);
        if (m_num_advances < m_config.m_size)
        {
            ++m_num_advances;
        }
    }
    return *this;
}

template <typename ENTRY>
ENTRY & Record_table<ENTRY>::write_entry()
{
    if (!m_config.m_enabled)
    {
        return m_dummy_entry;
    }
    return *m_write;
}

template <typename ENTRY>
ENTRY * Record_table<ENTRY>::next(ENTRY * entry) const
{
    auto n = ++entry;
    if (n == m_end)
    {
        // Rollover
        return m_entries;
    }
    return n;
}

// @pre no memory allocated currently.
template <typename ENTRY>
void Record_table<ENTRY>::allocate_entries()
{
    assert(m_entries == nullptr);

    m_entries = new ENTRY(m_config.m_size);
    m_end = m_entries + m_config.m_size;

    // Point to last entry, since client should advance before writing.
    m_write = m_end - 1;
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
void Record_table<ENTRY>::clear()
{
    m_num_advances = 0;
    m_write = m_end - 1;
}

template <typename ENTRY>
bool Record_table<ENTRY>::enable(bool enable)
{
    if (enable)
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
    m_config.m_enabled = enable;
    return true;
}

template <typename ENTRY>
bool Record_table<ENTRY>::size(unsigned size)
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
