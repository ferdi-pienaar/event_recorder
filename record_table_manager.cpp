/*
 *
 *
 */

#include "record_table_manager.h"
#include "record_table_generic.h"

Record_table_manager::Record_table_manager(const std::map<std::string, Record_table_generic &> & table_map) : m_tables(table_map)
{
}

/// @brief 
/// @param name 
void Record_table_manager::callback_entries(std::string name) const
{
    auto iter = m_tables.find(name);
    if (iter == m_tables.end())
    {
        return;
    }

    auto const & table = iter->second;
    table.do_callback();
}
