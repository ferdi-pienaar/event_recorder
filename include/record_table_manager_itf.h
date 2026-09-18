/*
 *
 */
#pragma once
#include <functional>
#include <string>

namespace Event_record
{

// The interface of class Table_manager.
class Table_manager_interface
{
public:
    virtual bool dump_tables(const std::function<bool(const std::string &)> &) const = 0;
    virtual bool enable_tables(const std::function<bool(const std::string &)> &,
                               bool ena) const = 0;
    virtual bool oneshot_tables(const std::function<bool(const std::string &)> &,
                                bool one) const = 0;
    virtual bool size_tables(const std::function<bool(const std::string &)> &,
                             unsigned size) const = 0;
    virtual bool clear_tables(const std::function<bool(const std::string &)> &) const = 0;
    virtual bool dump_tables_state(const std::function<bool(const std::string &)> &) const = 0;
};

} // namespace Event_record
