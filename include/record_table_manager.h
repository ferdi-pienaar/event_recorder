/*
 * Record_table_manager allows client operator code to manage groups of Record_table,
 * if all the Record_table have the same type of ENTRY.
 * Each managed Record_table has a name, and Record_table manager can apply
 * operator actions (enable, dump, re-size, clear, etc) to Record_tables whose
 * names match a string input by the operator.
 *
 * The implementer of the operator code should therefore assign names to
 * the tables such that sub-strings can be used to identify groups of tables
 * by name, e.g. names of tables pertaining to upstream should contain "upstream",
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
#include "record_table_manager_itf.h"

class Record_table_op_itf;

class Record_table_manager: public Record_table_manager_interface
{
public:
    using DUMP_NAME_CALLBACK = void (*)(const std::string &);

    Record_table_manager(const std::map<std::string, Record_table_op_itf &> &,
                         DUMP_NAME_CALLBACK dump_name = nullptr);
    void dump_tables(std::string substring) const override;
    void enable_tables(std::string substring, bool ena) const override;
    void oneshot_tables(std::string substring, bool one) const override;
    void size_tables(std::string substring, unsigned size) const override;
    void clear_tables(std::string substring) const override;
    void dump_tables_state(std::string substring) const override;

private:
    void do_tables(std::string substring, std::function<void(Record_table_op_itf &)>) const;

    std::map<std::string, Record_table_op_itf &> m_tables;
    const DUMP_NAME_CALLBACK m_dump_name = nullptr;
};
