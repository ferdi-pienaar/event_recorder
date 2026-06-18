/*
 */

#include "record_table_manager.h"
#include "record_table_op_itf.h"

// xxx nothing prevents client inserting the same Record_table more than once, with a different name.
//template <typename ENTRY>
Record_table_manager::Record_table_manager(const std::map<std::string, Record_table_op_itf &> & tables,
        DUMP_NAME_CALLBACK dump_name) :
    m_tables(tables), m_dump_name(dump_name)
{
}

// xxx should tables be disabled?
// It seems useful since we don't know what may be most convenient for client operator...
void Record_table_manager::dump_tables(std::string substring) const
{
    do_tables(substring, [](Record_table_op_itf & t) { t.dump(); });
}

void Record_table_manager::enable_tables(std::string substring, bool enable) const
{
    do_tables(substring, [&](Record_table_op_itf & t) { t.enable(enable); });
}

void Record_table_manager::oneshot_tables(std::string substring, bool one) const
{
    do_tables(substring, [&](Record_table_op_itf & t) { t.oneshot(one); });
}

// xxx can fail if a table a table is enabled.
void Record_table_manager::size_tables(std::string substring, unsigned size) const
{
    do_tables(substring, [&](Record_table_op_itf & t) { t.set_size(size); });
}

// xxx can fail if a table a table is enabled.
void Record_table_manager::clear_tables(std::string substring) const
{
    do_tables(substring, [](Record_table_op_itf & t) { t.clear(); });
}

void Record_table_manager::dump_tables_state(std::string substring) const
{
    do_tables(substring, [](Record_table_op_itf & t) { t.dump_state(); });
}

// Call table_fn for all tables in this manager that have names that match substring.
// xxx could we return an error string for a failed operation?
void Record_table_manager::do_tables(std::string substring, std::function<void(Record_table_op_itf &)> table_fn) const
{
    // Lambda returns true if substring is in the Record_table's key.
    auto matcher = [&](const std::pair<std::string, Record_table_op_itf &> & item)
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

        // Output the name of the table so client knows which tables matched its input string.
        if (m_dump_name != nullptr)
        {
            m_dump_name(iter->first);
        }
        auto & table = iter->second;
        table_fn(table);
        // Next search starts at next item.
        start = ++iter;
    }
}
