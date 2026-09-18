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
#include <map>

namespace Event_record
{

class Table_op_itf;

class Table_manager : public Table_manager_interface
{
public:
    using OPERATOR_OUTPUT_CALLBACK = std::function<void(const std::string &)>;

    Table_manager(const std::map<std::string, Table_op_itf &> &,
                  OPERATOR_OUTPUT_CALLBACK op_out = nullptr);
    bool dump_tables(const std::function<bool(const std::string &)> &) const override;
    bool enable_tables(const std::function<bool(const std::string &)> &, bool ena) const override;
    bool oneshot_tables(const std::function<bool(const std::string &)> &, bool one) const override;
    bool size_tables(const std::function<bool(const std::string &)> &,
                     unsigned size) const override;
    bool clear_tables(const std::function<bool(const std::string &)> &) const override;
    bool dump_tables_state(const std::function<bool(const std::string &)> &) const override;

private:
    bool do_tables(const std::function<bool(const std::string &)> &,
                   const std::function<bool(Table_op_itf &)> &) const;

    std::map<std::string, Table_op_itf &> m_tables;
    const OPERATOR_OUTPUT_CALLBACK m_operator_out = nullptr;
};

} // namespace Event_record
