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
    using DUMP_NAME_CALLBACK = void (*)(const std::string &);

    Record_table_manager(const std::map<std::string, Record_table_generic &> &,
                         DUMP_NAME_CALLBACK dump_name = nullptr);
    void dump_tables(std::string substring) const;
    void enable_tables(std::string substring, bool ena) const;
    void oneshot_tables(std::string substring, bool one) const;
    void size_tables(std::string substring, unsigned size) const;
    void clear_tables(std::string substring) const;
    void dump_tables_state(std::string substring) const;

private:
    void do_tables(std::string substring, std::function<void(Record_table_generic &)>) const;

    std::map<std::string, Record_table_generic &> m_tables;
    const DUMP_NAME_CALLBACK m_dump_name = nullptr;
};
