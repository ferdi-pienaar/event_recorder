/*
 * Event_record::Table_manager allows client operator code to manage multiple Event_record::Table.
 * Each managed Event_record::Table has a name, and Table_manager can apply operator actions
 * (enable, dump, re-size, clear, etc) to Event_record::Tables whose names match a string input by
 * the operator.
 *
 * The implementer of the operator code should therefore assign names to the tables such that
 * sub-strings can be used to identify groups of tables by name, e.g. names of tables pertaining to
 * upstream should contain "upstream", and names of tables pertaining to protocol DHCP should
 * contain "dhcp", and names of tables pertaining to invalid packets should contain "invalid", etc.
 * This would result in names like "invalid-upstream-dhcp", "valid-downstream-dns", etc. It would
 * allow the operator to enable all tables related to DHCP with one command, or re-size all tables
 * related to downstream packets with one command.
 */
#pragma once
#include "record_table_manager_itf.h"
#include <functional>
#include <map>
#include <string>

namespace Event_record
{

class Table_op_itf;

class Table_manager : public Table_manager_interface
{
public:
    using OPERATOR_OUTPUT_CALLBACK = std::function<void(const std::string &)>;

    Table_manager(const std::map<std::string, Table_op_itf &> &,
                  OPERATOR_OUTPUT_CALLBACK op_out = nullptr);
    bool dump_tables(std::string substring) const override;
    bool enable_tables(std::string substring, bool ena) const override;
    bool oneshot_tables(std::string substring, bool one) const override;
    bool size_tables(std::string substring, unsigned size) const override;
    bool clear_tables(std::string substring) const override;
    bool dump_tables_state(std::string substring) const override;

private:
    bool do_tables(std::string substring, std::function<bool(Table_op_itf &)>) const;

    std::map<std::string, Table_op_itf &> m_tables;
    const OPERATOR_OUTPUT_CALLBACK m_operator_out = nullptr;
};

} // namespace Event_record
