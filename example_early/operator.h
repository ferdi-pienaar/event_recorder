/*
 */
#pragma once

class Record_table_manager_interface;

class Operator
{
public:
    Operator(const Record_table_manager_interface &mgr) : m_mgr(mgr) {}
    void run();

private:
    bool handle_command();

    const Record_table_manager_interface &m_mgr;
};

