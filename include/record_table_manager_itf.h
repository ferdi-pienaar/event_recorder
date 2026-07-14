/*
 *
 */
#pragma once
#include <string>

namespace Event_record
{

// The interface of class Table_manager.
class Table_manager_interface
{
public:
    virtual bool dump_tables(std::string substring) const = 0;
    virtual bool enable_tables(std::string substring, bool ena) const = 0;
    virtual bool oneshot_tables(std::string substring, bool one) const = 0;
    virtual bool size_tables(std::string substring, unsigned size) const = 0;
    virtual bool clear_tables(std::string substring) const = 0;
    virtual bool dump_tables_state(std::string substring) const = 0;
};

} // namespace Event_record
