/*
 * Record_table_manager allows client operator code to manage groups of Record_table,
 * if all the Record_table have the same type of ENTRY.
 * Each managed Record_table has a name, and Record_table manager can apply
 * operator actions (enable, dump, re-size, clear, etc) to Record_tables whose
 * names match a string input by the operator.
 *
 * The implementer of the operator code should therefore assign names to
 * the tables such that sub-strings can be used to identify groups of tables
 * by name, e.g. names of the tables pertaining to upstream should contain "upstream",
 * and names of tables pertaining to protocol DHCP should contain "dhcp",
 * and names of tables pertaining to invalid packets should contain "invalid", etc.
 * This would result in names like "invalid-upstream-dhcp", "valid-downstream-dns", etc.
 * It would allow the operator to enable all tables related to DHCP with one command, or
 * re-size all tables related to downstream packets with one command.
 *
 */

#pragma once
#include <string>
#include <map>
#include <functional>
#include "record_table.h"
#include "record_table_manager_itf.h"

template <typename ENTRY>
class Record_table_manager: public Record_table_manager_interface
{
public:
    using DUMP_NAME_CALLBACK = void (*)(const std::string &);

    Record_table_manager(const std::map<std::string, Record_table<ENTRY> &> &,
                         DUMP_NAME_CALLBACK dump_name = nullptr);
    Record_table<ENTRY> * get_table(const std::string & name);
    bool handle_cmd(char cmd, const std::string & substring, unsigned param = 0) const;
    void dump_tables(std::string substring) const override;
    void enable_tables(std::string substring, bool ena) const override;
    void oneshot_tables(std::string substring, bool one) const override;
    void size_tables(std::string substring, unsigned size) const override;
    void clear_tables(std::string substring) const override;
    void dump_tables_state(std::string substring) const override;

private:
    void do_tables(std::string substring, std::function<void(Record_table<ENTRY> &)>) const;

    std::map<std::string, Record_table<ENTRY> &> m_tables;
    const DUMP_NAME_CALLBACK m_dump_name = nullptr;
};

// xxx nothing prevents client inserting the same Record_table more than once, with a different name.
template <typename ENTRY>
Record_table_manager<ENTRY>::Record_table_manager(const std::map<std::string, Record_table<ENTRY> &> & tables,
        DUMP_NAME_CALLBACK dump_name) :
    m_tables(tables), m_dump_name(dump_name)
{
}

// Used by 'recorder' client to get a table by name.
template <typename ENTRY>
Record_table<ENTRY> * Record_table_manager<ENTRY>::get_table(const std::string & name)
{
    auto iter = m_tables.find(name);
    if (iter == m_tables.end())
    {
        return nullptr;
    }
    return &iter->second;
}

// Operator can use this to handle commands, or it can directly use the virtual functions dump_tables, etc.
// xxx is it useful to have these 2 APIs, one using virtual functions and the other this command handler?
template <typename ENTRY>
bool Record_table_manager<ENTRY>::handle_cmd(char cmd, const std::string & substring, unsigned param) const
{
    switch (cmd)
    {
    case 'e': // enable/disable
        enable_tables(substring, param);
        return true;
    case 's': // size
        size_tables(substring, param);
        return true;
    case 'o': // oneshot/rollover
        oneshot_tables(substring, param);
        return true;
    case 'd': // dump
        dump_tables(substring);
        return true;
    case 'c': // clear
        clear_tables(substring);
        return true;
    case 't': // tables state
        dump_tables_state(substring);
        return true;
    default:
        return false;
    }
}

// xxx should tables be disabled?
// It seems useful since we don't know what may be most convenient for client operator...
template <typename ENTRY>
void Record_table_manager<ENTRY>::dump_tables(std::string substring) const
{
    do_tables(substring, [](Record_table<ENTRY> & t) { t.dump(); });
}

template <typename ENTRY>
void Record_table_manager<ENTRY>::enable_tables(std::string substring, bool enable) const
{
    do_tables(substring, [&](Record_table<ENTRY> & t) { t.enable(enable); });
}

template <typename ENTRY>
void Record_table_manager<ENTRY>::oneshot_tables(std::string substring, bool one) const
{
    do_tables(substring, [&](Record_table<ENTRY> & t) { t.oneshot(one); });
}

// xxx can fail if a table a table is enabled.
template <typename ENTRY>
void Record_table_manager<ENTRY>::size_tables(std::string substring, unsigned size) const
{
    do_tables(substring, [&](Record_table<ENTRY> & t) { t.size(size); });
}

// xxx can fail if a table a table is enabled.
template <typename ENTRY>
void Record_table_manager<ENTRY>::clear_tables(std::string substring) const
{
    do_tables(substring, [](Record_table<ENTRY> & t) { t.clear(); });
}

template <typename ENTRY>
void Record_table_manager<ENTRY>::dump_tables_state(std::string substring) const
{
    do_tables(substring, [](Record_table<ENTRY> & t) { t.dump_state(); });
}

// xxx could we return an error string for a failed operation?
// Call table_fn for all tables that match substring.
template <typename ENTRY>
void Record_table_manager<ENTRY>::do_tables(std::string substring, std::function<void(Record_table<ENTRY> &)> table_fn) const
{
    // Lambda returns true if substring is in the Record_table's key.
    auto matcher = [&](const std::pair<std::string, Record_table<ENTRY> &> & item)
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
