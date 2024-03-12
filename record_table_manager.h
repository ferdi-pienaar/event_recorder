/*
 *
 *
 */

#pragma once
#include <string>
#include <map>
#include <functional>

class Record_table_generic;

class Record_table_manager
{
public:
    Record_table_manager(const std::map<std::string, Record_table_generic &> &);
    void dump_tables(std::string substring) const;
    void enable_tables(std::string substring, bool ena) const;
    void oneshot_tables(std::string substring, bool one) const;
    void size_tables(std::string substring, unsigned size) const;
    void clear_tables(std::string substring) const;
    void dump_tables_state(std::string substring) const;

private:
    void do_tables(std::string substring, std::function<void(Record_table_generic &)>) const;

    std::map<std::string, Record_table_generic &> m_tables;
};
