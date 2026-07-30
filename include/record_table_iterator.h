/*
 * Iterator is used by Event_record::Table::dump to iterate through the written ENTRYs of a table.
 * Optionally, the operator can use Event_record::Table_iterator directly to examine written ENTRYs
 * in a table, e.g. to implement a dump feature that only dumps a range of entries.
 */
#pragma once
#include "rollover_type.h"

namespace Event_record
{

template <typename ENTRY, RolloverType RT> class Table;

// Iterate over written entries in Event_record::Table, from oldest to newest.
template <typename ENTRY, RolloverType RT = RolloverType::RuntimeSelect> class Table_iterator
{
public:
    Table_iterator(const Table<ENTRY, RT> &table);
    void begin() noexcept;
    void next() noexcept;
    const ENTRY &get_current() noexcept;
    bool end() const noexcept;

private:
    const Table<ENTRY, RT> &m_table;
    ENTRY *m_current = nullptr;
    unsigned m_entries_remain = 0; // The number of entries we still have to advance.
};

template <typename ENTRY, RolloverType RT>
Table_iterator<ENTRY, RT>::Table_iterator(const Table<ENTRY, RT> &table) : m_table(table)
{
}

template <typename ENTRY, RolloverType RT> void Table_iterator<ENTRY, RT>::begin() noexcept
{
    if (m_table.m_num_written_entries == m_table.m_config.get_size())
    {
        // All entries filled, so start with next to be written.
        m_current = m_table.m_write;
    }
    else
    {
        // All entries not filled, so start with the first entry in the array.
        m_current = m_table.m_entries;
    }

    m_entries_remain = m_table.m_num_written_entries;
}

template <typename ENTRY, RolloverType RT> void Table_iterator<ENTRY, RT>::next() noexcept
{
    m_current = m_table.next(m_current);
    --m_entries_remain;
}

template <typename ENTRY, RolloverType RT> const ENTRY &Table_iterator<ENTRY, RT>::get_current() noexcept
{
    return *m_current;
}

template <typename ENTRY, RolloverType RT> bool Table_iterator<ENTRY, RT>::end() const noexcept
{
    return m_entries_remain == 0;
}

} // namespace Event_record
