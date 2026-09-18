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
bool Table_manager::dump_tables(const std::function<bool(const std::string &)> &name_matcher) const
{
    return do_tables(name_matcher, [](Table_op_itf &t) { return t.dump(); });
}

bool Table_manager::enable_tables(const std::function<bool(const std::string &)> &name_matcher,
                                  bool enable) const
{
    return do_tables(name_matcher, [&](Table_op_itf &t) { return t.enable(enable); });
}

bool Table_manager::oneshot_tables(const std::function<bool(const std::string &)> &name_matcher,
                                   bool one) const
{
    return do_tables(name_matcher, [&](Table_op_itf &t) { return t.oneshot(one); });
}

bool Table_manager::size_tables(const std::function<bool(const std::string &)> &name_matcher,
                                unsigned size) const
{
    return do_tables(name_matcher, [&](Table_op_itf &t) { return t.set_size(size); });
}

bool Table_manager::clear_tables(const std::function<bool(const std::string &)> &name_matcher) const
{
    return do_tables(name_matcher, [](Table_op_itf &t) { return t.clear(); });
}

bool Table_manager::dump_tables_state(
    const std::function<bool(const std::string &)> &name_matcher) const
{
    return do_tables(name_matcher, [](Table_op_itf &t) { return t.dump_state(); });
}

// Call table_fn for all tables in this manager that have names that return true from matcher.
// xxx could we return an error string for a failed operation?
bool Table_manager::do_tables(const std::function<bool(const std::string &)> &name_matcher,
                              const std::function<bool(Table_op_itf &)> &table_fn) const
{
    // Lambda returns true iff the user-provided name_matcher matches the Table's name.
    auto matcher = [&](const std::pair<std::string, Table_op_itf &> &item)
    { return name_matcher(item.first); };

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
