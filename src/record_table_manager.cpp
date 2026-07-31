/*
 * This module depends only on the operator interface of Event_record::Table.
 */
#include "record_table_manager.h"
#include "record_table_op_itf.h"
#include <sstream>

namespace Event_record
{

Table_manager::Table_manager(const std::map<std::string, Table_op_itf &> &tables,
                             OPERATOR_OUTPUT_CALLBACK op_out)
    : m_tables(tables), m_operator_out(op_out)
{
}

// xxx should tables be disabled?
// It seems useful since we don't know what may be most convenient for client operator...
bool Table_manager::dump_tables(std::string substring) const
{
    return do_tables(substring, [](Table_op_itf &t) { return t.dump(); });
}

bool Table_manager::enable_tables(std::string substring, bool enable) const
{
    return do_tables(substring, [&](Table_op_itf &t) { return t.enable(enable); });
}

bool Table_manager::oneshot_tables(std::string substring, bool one) const
{
    return do_tables(substring, [&](Table_op_itf &t) { return t.oneshot(one); });
}

bool Table_manager::size_tables(std::string substring, unsigned size) const
{
    return do_tables(substring, [&](Table_op_itf &t) { return t.set_size(size); });
}

bool Table_manager::clear_tables(std::string substring) const
{
    return do_tables(substring, [](Table_op_itf &t) { return t.clear(); });
}

bool Table_manager::dump_tables_state(std::string substring) const
{
    return do_tables(substring, [](Table_op_itf &t) { return t.dump_state(); });
}

// Call table_fn for all tables in this manager that have names that match substring.
// xxx could we return an error string for a failed operation?
bool Table_manager::do_tables(std::string substring,
                              std::function<bool(Table_op_itf &)> table_fn) const
{
    // Lambda returns true if substring is in the Table's key.
    auto matcher = [&](const std::pair<std::string, Table_op_itf &> &item)
    {
        auto pos = item.first.find(substring);
        return pos != std::string::npos;
    };

    bool result = true; // Success, unless something fails below.
    for (auto iter = m_tables.begin();; ++iter)
    {
        // Search remaining items in map.
        iter = std::find_if(iter, m_tables.end(), matcher);
        if (iter == m_tables.end())
        {
            break;
        }

        // Found a matching table: output its name so client knows which tables matched its input.
        if (m_operator_out != nullptr)
        {
            std::ostringstream out;
            out << "Table '" << iter->first << "'" << std::endl;
            m_operator_out(out.str());
        }
        auto &table = iter->second;
        if (table_fn(table) == false)
        {
            if (m_operator_out != nullptr)
            {
                std::ostringstream out;
                out << "Operation failed on table '" << iter->first << "'" << std::endl;
                m_operator_out(out.str());
            }
            result = false;
        }
    }
    return result;
}

} // namespace Event_record
