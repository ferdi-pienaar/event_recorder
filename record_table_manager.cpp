/*
 *
 *
 */

#include "record_table_manager.h"
#include "record_table_generic.h"
#include <algorithm>
#include <iostream>


// xxx nothing prevents client inserting the same Record_table more than once.
Record_table_manager::Record_table_manager(const std::map<std::string, Record_table_generic &> & table_map) : m_tables(table_map)
{
}

void Record_table_manager::dump_tables(std::string substring) const
{
    do_tables(substring, [](Record_table_generic & t){ t.dump(); });
}

void Record_table_manager::enable_tables(std::string substring, bool enable) const
{
    do_tables(substring, [&](Record_table_generic & t){ t.enable(enable); });
}

void Record_table_manager::size_tables(std::string substring, unsigned size) const
{
    do_tables(substring, [&](Record_table_generic & t){ t.size(size); });
}

void Record_table_manager::clear_tables(std::string substring) const
{
    do_tables(substring, [&](Record_table_generic & t){ t.clear(); });
}

// xxx could we return an error string for a failed operation?
void Record_table_manager::do_tables(std::string substring, std::function<void(Record_table_generic &)> table_fn) const
{    
    // Lambda returns true if substring is in the entry's key.
    auto matcher = [&](const std::pair<std::string, Record_table_generic &> & item)
    {
        auto pos = item.first.find(substring);
        return pos != std::string::npos;
    };

    for (auto start = m_tables.begin(); ; )
    {
        // Search remaining items in map.
        auto iter = std::find_if(start, m_tables.end(), matcher);
        if (iter == m_tables.end())
        {
            break;
        }
        auto & table = iter->second;
        table_fn(table);
        // Next search starts at next item.
        start = ++iter;
    }  
}
