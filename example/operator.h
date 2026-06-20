/*
 *
 */
#pragma once

class Record_table_manager_interface;

class Operator
{
public:
    void init(const Record_table_manager_interface &mgr);

private:
    bool handle_command(const Record_table_manager_interface &mgr);
};

