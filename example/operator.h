/*
 */
#pragma once

namespace Event_record
{
class Table_manager_interface;
}

class Operator
{
public:
    Operator(const Event_record::Table_manager_interface &mgr) : m_mgr(mgr) {}
    void run();

private:
    bool handle_command();

    const Event_record::Table_manager_interface &m_mgr;
};
