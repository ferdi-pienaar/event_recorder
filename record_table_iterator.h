/*
 * Iterator is used by Record_table::dump to iterate through
 * the written ENTRYs of a table.
 *
 */

#pragma once
#include "record_table.h"

// Iterate over written entries in Record_table, from oldest to newest.
template <typename ENTRY>
class Record_table_iterator
{
public:
    Record_table_iterator(const Record_table<ENTRY> & table);
    void begin() noexcept;
    void next() noexcept;
    const ENTRY & get_current() noexcept;
    bool end() const noexcept;

private:
    const Record_table<ENTRY> & m_table;
    ENTRY * m_current = nullptr;
    ENTRY * m_end = nullptr;
    bool m_moved = false;
};

template <typename ENTRY>
Record_table_iterator<ENTRY>::Record_table_iterator(const Record_table<ENTRY> & table) : m_table(table)
{
}

template <typename ENTRY>
void Record_table_iterator<ENTRY>::begin() noexcept
{
    if (m_table.m_num_advances == m_table.m_config.m_size)
    {
        // All entries filled, so start with next to be written,
        // and end when we've returned there.
        m_current = m_table.m_write;
        m_end = m_current;
    }
    else
    {
        // All entries not filled, so start with the first entry in the array,
        // and end at the next to be written.
        m_current = m_table.m_entries;
        m_end = m_table.m_write;
    }
    m_moved = false;
}

template <typename ENTRY>
void Record_table_iterator<ENTRY>::next() noexcept
{
    m_current = m_table.next(m_current);
    m_moved = true;
}

template <typename ENTRY>
const ENTRY & Record_table_iterator<ENTRY>::get_current() noexcept
{
    return *m_current;
}

template <typename ENTRY>
bool Record_table_iterator<ENTRY>::end() const noexcept
{
    if (m_table.m_num_advances == 0)
    {
        // No entries written.
        return true;
    }
    return m_moved && (m_current == m_end);
}
