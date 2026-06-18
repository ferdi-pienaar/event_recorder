/*
 *
 */

#pragma once
#include <string>

// The interface of class Record_table_manager.
class Record_table_manager_interface
{
public:
    virtual void dump_tables(std::string substring) const = 0;
    virtual void enable_tables(std::string substring, bool ena) const = 0;
    virtual void oneshot_tables(std::string substring, bool one) const = 0;
    virtual void size_tables(std::string substring, unsigned size) const = 0;
    virtual void clear_tables(std::string substring) const = 0;
    virtual void dump_tables_state(std::string substring) const = 0;

private:
};
