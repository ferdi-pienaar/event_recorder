/*
 *
 *
 */

#pragma once
#include <string>
#include <map>

class Record_table_generic;

class Record_table_manager
{
public:
    Record_table_manager(const std::map<std::string, Record_table_generic &> &);
    void callback_entries(std::string name) const;

private:
    std::map<std::string, Record_table_generic &> m_tables;
};
