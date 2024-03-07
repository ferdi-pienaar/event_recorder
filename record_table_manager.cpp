/*
 *
 *
 */

#include "record_table_manager.h"

Record_table_manager::Record_table_manager(const std::map<std::string, Record_table_generic &> & table_map) : m_tables(table_map)
{
}
